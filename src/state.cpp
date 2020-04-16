#include <cassert>
#include <memory>
#include <new>
#include <stdexcept>
#include <unordered_map>
#include <utility>

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

#include <cool/defer.hpp>
#include <cool/indices.hpp>
#include <luapp/state.hpp>
#include <luapp/tuple.hpp>
#include <luapp/type.hpp>
#include <luapp/value.hpp>

namespace lua
{

state::state(options opt)
  : data_(
      new state_data{luaL_newstate(), {}}, +[](state_data* data) {
        assert(lua_gettop(data->state) == 0);
        lua_close(data->state);
        delete data;
      })
{
  if (!data_ || !data_->state)
    throw std::bad_alloc{};

  const auto state = data_->state;

  if (opt & options::std_libs)
    luaL_openlibs(state);

  const auto push_gc_table = [&](const char* marker, int (*f)(lua_State*)) {
    lua_createtable(state, 0, 2);

    lua_pushstring(state, "__gc");
    lua_pushcfunction(state, f);
    lua_rawset(state, -3);

    lua_pushstring(state, marker);
    lua_pushboolean(state, true);
    lua_rawset(state, -3);
  };

  {
    push_gc_table(
      "__luapp_userdata", +[](lua_State* state) -> int {
        auto* p = reinterpret_cast<std::any*>(lua_touserdata(state, -1));
        p->~any();
        return 0;
      });
    reference ref(data_, luaL_ref(state, LUA_REGISTRYINDEX));
    data_->metatables.insert_or_assign(typeid(void), table(std::move(ref)));
  }

  {
    using fn_t = std::shared_ptr<std::function<tuple(tuple)>>;
    push_gc_table(
      "__luapp_function", +[](lua_State* state) -> int {
        auto* p = reinterpret_cast<fn_t*>(lua_touserdata(state, -1));
        p->~fn_t();
        return 0;
      });
    reference ref(data_, luaL_ref(state, LUA_REGISTRYINDEX));
    data_->metatables.insert_or_assign(typeid(fn_t), table(std::move(ref)));
  }

  {
    using weak_state = std::weak_ptr<state_data>;
    push_gc_table(
      "__luapp_weakstate", +[](lua_State* state) -> int {
        auto* p = reinterpret_cast<weak_state*>(lua_touserdata(state, -1));
        p->~weak_state();
        return 0;
      });
    reference ref(data_, luaL_ref(state, LUA_REGISTRYINDEX));
    data_->metatables.insert_or_assign(typeid(weak_state), table(std::move(ref)));
  }
}

auto state::global_table() const -> table
{
  const auto state = data_->state;

  if (!lua_checkstack(state, 1))
    throw std::bad_alloc{};

  lua_pushglobaltable(state);
  reference ref(data_, luaL_ref(state, LUA_REGISTRYINDEX));

  return table(std::move(ref));
}

auto state::do_string(const char* code) const -> tuple
{
  const auto state = data_->state;
  if (!lua_checkstack(state, 1))
    throw std::bad_alloc{};

  const auto last_top = lua_gettop(state);

  if (luaL_loadstring(state, code) || lua_pcall(state, 0, LUA_MULTRET, 0)) {
    std::string error = lua_tostring(state, -1);
    lua_pop(state, 1);
    throw std::runtime_error{std::move(error)};
  }

  const auto n = lua_gettop(state) - last_top;
  COOL_DEFER(lua_pop(state, n));

  tuple result;
  result.resize(n);

  for (const auto i : cool::closed_indices(1, n))
    result[i - 1] = value::at(data_, last_top + i);

  return result;
}

auto state::get_metatable(std::type_index key) const -> const table&
{
  auto& mt = data_->metatables;

  if (const auto it = mt.find(key); it != mt.end())
    return it->second;

  const auto [it, _] = mt.insert_or_assign(key, mt.at(typeid(void)));
  return it->second;
}

auto state::create_table() const -> table
{
  const auto state = data_->state;
  lua_createtable(state, 0, 0);
  reference ref(data_, luaL_ref(state, LUA_REGISTRYINDEX));
  return table(std::move(ref));
}

auto state::register_metatable(std::type_index tidx,
                               std::initializer_list<std::pair<value, value>> entries) const -> void
{
  auto& mts = data_->metatables;
  if (mts.find(tidx) != mts.end())
    throw std::runtime_error{"metatable already exists."};

  table mt = create_table();
  table gct(mts.at(typeid(void)));

  set(mt, "__luapp_userdata", true);
  set(mt, "__gc", get(gct, "__gc"));

  for (const auto& [key, value] : entries) {
    if (key == "__luapp_userdata" || key == "__gc")
      throw std::runtime_error{"invalid key: " + key.get_string_or("")};
    set(mt, key, value);
  }

  mts.insert_or_assign(tidx, std::move(mt));
}

} // namespace lua
