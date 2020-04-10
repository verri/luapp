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

table::table(std::shared_ptr<lua_State> state, int ref) : table(reference{std::move(state), ref}) {}

auto table::get(std::string_view key) const -> reference
{
  const auto state = ref_.state();
  if (!lua_checkstack(state, 2))
    throw std::bad_alloc{};

  push();
  COOL_DEFER(lua_pop(state, 1));

  lua_pushlstring(state, key.data(), key.size());
  lua_gettable(state, -2);

  return reference(ref_.sstate(), luaL_ref(state, LUA_REGISTRYINDEX));
}

auto table::set(std::string_view key, const value& value) -> void
{
  const auto state = ref_.state();
  if (!lua_checkstack(state, 3))
    throw std::bad_alloc{};

  push();
  COOL_DEFER(lua_pop(state, 1));

  lua_pushlstring(state, key.data(), key.size());
  value.push(state);
  lua_settable(state, -3);
}

auto table::get(const value& key) const -> reference
{
  const auto state = ref_.state();
  if (!lua_checkstack(state, 2))
    throw std::bad_alloc{};

  push();
  COOL_DEFER(lua_pop(state, 1));

  key.push(state);
  lua_gettable(state, -2);

  return reference(ref_.sstate(), luaL_ref(state, LUA_REGISTRYINDEX));
}

auto table::set(const value& key, const value& value) -> void
{
  const auto state = ref_.state();
  if (!lua_checkstack(state, 3))
    throw std::bad_alloc{};

  push();
  COOL_DEFER(lua_pop(state, 1));

  key.push(state);
  value.push(state);
  lua_settable(state, -3);
}

auto table::push() const -> int { return push(ref_.state()); }

auto table::push(lua_State* state) const -> int
{
  assert(state == ref_.state());
  if (!lua_checkstack(state, 1))
    throw std::bad_alloc{};

  ref_.push();
  return LUA_TTABLE;
}

} // namespace lua
