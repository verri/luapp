#include <new>
#include <utility>

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

#include <luapp/state.hpp>

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
  const auto index = luaL_ref(state_.get(), LUA_REGISTRYINDEX);
  return table(state_, index);
}

} // namespace lua
