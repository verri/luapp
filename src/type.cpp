#include <luapp/type.hpp>

namespace lua
{

userdata::userdata(std::any data, table metatable) noexcept
  : data_(std::move(data)), metatable_(std::move(metatable))
{}

// XXX:
auto userdata::push(lua_State* state) const -> int
{
  auto p = lua_newuserdata(state, sizeof(std::any));
  new (p) std::any(data_);
  // TODO: push metatable
  return LUA_TUSERDATA;
}

} // namespace lua
