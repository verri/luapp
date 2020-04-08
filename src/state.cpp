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

} // namespace lua
