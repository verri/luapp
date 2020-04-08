#include <new>

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

#include <cool/defer.hpp>
#include <luapp/reference.hpp>

namespace lua
{

reference::reference() noexcept : state_{nullptr}, index_{LUA_NOREF} {}

reference::~reference() noexcept
{
  if (state_)
    luaL_unref(state_, LUA_REGISTRYINDEX, index_);
}

reference::operator value() const
{
  if (!state_)
    return nil{};

  if (!lua_checkstack(state_, 1))
    throw std::bad_alloc{};

  const auto type = lua_rawgeti(state_, LUA_REGISTRYINDEX, index_);
  COOL_DEFER(lua_pop(state_, 1));

  switch (type)
  {
    case LUA_TNUMBER:
      if (lua_isinteger(state_, -1))
        return integer{lua_tointeger(state_, -1)};
      else
        return floating{lua_tonumber(state_, -1)};
    case LUA_TBOOLEAN:
      return boolean{static_cast<bool>(lua_toboolean(state_, -1))};
    case LUA_TSTRING:
      return string{lua_tostring(state_, -1)};
    case LUA_TTABLE:
      // TODO...
    case LUA_TFUNCTION:
      // TODO...
    case LUA_TUSERDATA:
      // TODO...
    case LUA_TLIGHTUSERDATA: // XXX:
    case LUA_TTHREAD: // XXX:
    case LUA_TNIL:
    default:
      return nil{};
  }
}

} // namespace lua
