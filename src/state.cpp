#include <new>
#include <stdexcept>
#include <utility>

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

#include <cool/defer.hpp>
#include <cool/indices.hpp>
#include <luapp/state.hpp>
#include <luapp/value.hpp>

namespace lua
{

state::state(options opt) : state_{luaL_newstate(), lua_close}
{
  if (!state_)
    throw std::bad_alloc{};

  if (opt & options::std_libs)
    luaL_openlibs(state_.get());
}

auto state::global_table() const -> table
{
  if (!lua_checkstack(state_.get(), 1))
    throw std::bad_alloc{};

  lua_pushglobaltable(state_.get());
  reference ref{state_};

  return table(std::move(ref));
}

auto state::do_string(const char* code) const -> tuple
{
  if (!lua_checkstack(state_.get(), 1))
    throw std::bad_alloc{};

  const auto state = state_.get();
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
    result[i - 1] = value::at(state_, last_top + i);

  return result;
}

} // namespace lua
