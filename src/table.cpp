#include <cassert>
#include <stdexcept>

extern "C" {
#include <lauxlib.h>
}

#include <cool/defer.hpp>
#include <luapp/table.hpp>
#include <luapp/value.hpp>

namespace lua
{

table::table(reference ref) noexcept : ref_(std::move(ref)) {}

auto table::get(const value& key) const -> reference
{
  const auto state = ref_.state();
  if (!lua_checkstack(state, 2))
    throw std::bad_alloc{};

  push(state);
  COOL_DEFER(lua_pop(state, 1));

  key.push(state);
  lua_gettable(state, -2);

  reference result{ref_.sstate()};
  return result;
}

auto table::set(const value& key, const value& value) const -> void
{
  const auto state = ref_.state();
  if (!lua_checkstack(state, 3))
    throw std::bad_alloc{};

  push(state);
  COOL_DEFER(lua_pop(state, 1));

  key.push(state);
  value.push(state);
  lua_settable(state, -3);
}

auto table::push(lua_State* state) const -> int
{
  assert(state == ref_.state());
  if (!lua_checkstack(state, 1))
    throw std::bad_alloc{};

  ref_.push(state);
  return LUA_TTABLE;
}

} // namespace lua
