#include <luapp/state.hpp>
#include <luapp/userdata.hpp>

namespace lua
{

userdata::userdata(const state& state, std::any data) noexcept
  : data_(std::move(data)), metatable_(state.get_metatable(data_.type()))
{}

userdata::userdata(std::any data, const reference& ref) noexcept
  : data_(std::move(data)), metatable_(ref)
{}

auto userdata::push(std::shared_ptr<state_data> state_data) const -> int
{
  const auto state = state_data->state;
  return push(std::move(state_data), state);
}

auto userdata::push(std::shared_ptr<state_data> state_data, lua_State* state) const -> int
{
  auto p = lua_newuserdata(state, sizeof(std::any));
  new (p) std::any(data_);

  metatable_.push(state_data, state);
  lua_setmetatable(state, -2);

  return LUA_TUSERDATA;
}

} // namespace lua
