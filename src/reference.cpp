#include <new>

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

#include <cool/defer.hpp>
#include <luapp/reference.hpp>

namespace lua
{

reference::reference() noexcept : index_{LUA_NOREF} {}

reference::~reference() noexcept
{
  if (const auto s = state_.lock())
    luaL_unref(s.get(), LUA_REGISTRYINDEX, index_);
}

reference::reference(std::weak_ptr<lua_State> state, int index) noexcept
  : state_{std::move(state)}, index_{index}
{}

reference::operator value() const
{
  const auto s = state_.lock();
  if (!s || !s.get())
    return nil{};

  const auto state = s.get();
  if (!lua_checkstack(state, 1))
    throw std::bad_alloc{};

  const auto type = lua_rawgeti(state, LUA_REGISTRYINDEX, index_);
  COOL_DEFER(lua_pop(state, 1));

  switch (type) {
  case LUA_TNUMBER:
    if (lua_isinteger(state, -1))
      return integer{lua_tointeger(state, -1)};
    else
      return floating{lua_tonumber(state, -1)};
  case LUA_TBOOLEAN:
    return boolean{static_cast<bool>(lua_toboolean(state, -1))};
  case LUA_TSTRING:
    return string{lua_tostring(state, -1)};
  case LUA_TTABLE:
    // TODO...
  case LUA_TFUNCTION:
    // TODO...
  case LUA_TUSERDATA:
    // TODO...
  case LUA_TLIGHTUSERDATA: // XXX:
  case LUA_TTHREAD:        // XXX:
  case LUA_TNIL:
  default:
    return nil{};
  }
}

} // namespace lua
