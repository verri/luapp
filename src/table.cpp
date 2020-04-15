#include <cassert>
#include <stdexcept>

extern "C" {
#include <lauxlib.h>
}

#include <cool/defer.hpp>
#include <luapp/state.hpp>
#include <luapp/table.hpp>
#include <luapp/value.hpp>

namespace lua
{

table::table(reference ref) noexcept : ref_(std::move(ref)) {}

auto table::get(const value& key) const -> reference
{
  const auto state_data = ref_.state();
  const auto state = state_data->state;

  if (!lua_checkstack(state, 2))
    throw std::bad_alloc{};

  push(state_data);
  COOL_DEFER(lua_pop(state, 1));

  key.push(state_data);
  lua_gettable(state, -2);

  return reference(state_data, luaL_ref(state, LUA_REGISTRYINDEX));
}

auto table::set(const value& key, const value& value) const -> void
{
  const auto state_data = ref_.state();
  const auto state = state_data->state;

  if (!lua_checkstack(state, 3))
    throw std::bad_alloc{};

  push(state_data);
  COOL_DEFER(lua_pop(state, 1));

  key.push(state_data);
  value.push(state_data);
  lua_settable(state, -3);
}

auto table::push(std::shared_ptr<state_data> state_data) const -> int
{
  const auto state = state_data->state;
  return push(std::move(state_data), state);
}

auto table::push(std::shared_ptr<state_data> state_data, lua_State* state) const -> int
{
  assert(state_data == ref_.state());

  if (!lua_checkstack(state, 1))
    throw std::bad_alloc{};

  ref_.push(std::move(state_data), state);
  return LUA_TTABLE;
}

auto table::operator==(const table& other) const -> bool
{
  // TODO
  return false;
}

auto table::operator!=(const table& other) const -> bool { return !(*this == other); }

} // namespace lua
