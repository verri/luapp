#include <cassert>

#include <cool/defer.hpp>
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

auto userdata::operator==(const userdata& other) const -> bool
{
  const auto sdata = metatable_.state();
  assert(sdata == other.metatable_.state());

  const auto state = sdata->state;

  push(sdata);
  COOL_DEFER(lua_pop(state, 1));

  other.push(sdata);
  COOL_DEFER(lua_pop(state, 1));

  return lua_compare(sdata->state, -1, -2, LUA_OPEQ);
}

auto userdata::operator!=(const userdata& other) const -> bool { return !(*this == other); }

} // namespace lua
