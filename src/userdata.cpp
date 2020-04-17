#include <cassert>
#include <typeindex>

extern "C" {
#include <lauxlib.h>
}

#include <cool/defer.hpp>
#include <luapp/state.hpp>
#include <luapp/userdata.hpp>

namespace lua
{

auto userdata::create_reference(const state& s, std::type_index tidx, std::any data) -> reference
{
  const auto sdata = s.data_;
  const auto& metatable = s.get_metatable(tidx);
  const auto state = sdata->state;

  auto p = lua_newuserdata(state, sizeof(std::any));
  new (p) std::any(std::move(data));

  metatable.push(sdata, state);
  lua_setmetatable(state, -2);

  return reference(sdata, luaL_ref(state, LUA_REGISTRYINDEX));
}

userdata::userdata(reference ref) noexcept : ref_(std::move(ref)) {}

userdata::userdata(const state& state, std::type_index tidx, std::any data)
  : ref_(create_reference(state, tidx, std::move(data)))
{}

auto userdata::push(std::shared_ptr<state_data> state_data) const -> int
{
  const auto state = state_data->state;
  return push(std::move(state_data), state);
}

auto userdata::push(std::shared_ptr<state_data> state_data, lua_State* state) const -> int
{
  assert(state_data->state == state);
  return ref_.push(std::move(state_data), state);
}

auto userdata::operator==(const userdata& other) const -> bool
{
  const auto sdata = ref_.state();
  assert(sdata == other.ref_.state());

  const auto state = sdata->state;

  push(sdata);
  COOL_DEFER(lua_pop(state, 1));

  other.push(sdata);
  COOL_DEFER(lua_pop(state, 1));

  return lua_compare(sdata->state, -1, -2, LUA_OPEQ);
}

auto userdata::operator!=(const userdata& other) const -> bool { return !(*this == other); }

auto userdata::data() const -> std::any*
{
  const auto state = ref_.state()->state;
  ref_.push(ref_.state(), state);
  COOL_DEFER(lua_pop(state, 1));
  return reinterpret_cast<std::any*>(lua_touserdata(state, -1));
}

} // namespace lua
