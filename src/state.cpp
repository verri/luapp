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

  const auto push_gc_table = [&](auto f) {
    lua_createtable(state, 0, 2);

    lua_pushstring(state, "__gc");
    lua_pushcfunction(state, f);
    lua_rawset(state, -3);

    lua_pushstring(state, "__luapp");
    lua_pushboolean(state, true);
    lua_rawset(state, -3);
  };

  {
    push_gc_table(+[](lua_State* state) -> int {
      auto* p = reinterpret_cast<std::any*>(lua_touserdata(state, -1));
      p->~any();
      return 0;
    });
    reference ref(data_, luaL_ref(state, LUA_REGISTRYINDEX));
    data_->metatables.insert_or_assign(typeid(void), std::move(ref));
  }

  {
    using fn_t = std::shared_ptr<std::function<tuple(tuple)>>;
    push_gc_table(+[](lua_State* state) -> int {
      auto* p = reinterpret_cast<fn_t*>(lua_touserdata(state, -1));
      p->~fn_t();
      return 0;
    });
    reference ref(data_, luaL_ref(state, LUA_REGISTRYINDEX));
    data_->metatables.insert_or_assign(typeid(fn_t), std::move(ref));
  }

  {
    using weak_state = std::weak_ptr<state_data>;
    push_gc_table(+[](lua_State* state) -> int {
      auto* p = reinterpret_cast<weak_state*>(lua_touserdata(state, -1));
      p->~weak_state();
      return 0;
    });
    reference ref(data_, luaL_ref(state, LUA_REGISTRYINDEX));
    data_->metatables.insert_or_assign(typeid(weak_state), std::move(ref));
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

auto state::get_metatable(std::type_index key) const -> const reference&
{
  const auto& mt = data_->metatables;

  const auto it = mt.find(key);
  return it != mt.end() ? it->second : mt.at(typeid(void));
}

auto state::create_table() const -> table
{
  const auto state = data_->state;
  lua_createtable(state, 0, 0);
  reference ref(data_, luaL_ref(state, LUA_REGISTRYINDEX));
  return table(std::move(ref));
}

} // namespace lua
