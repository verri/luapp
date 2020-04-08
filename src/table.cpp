#include <stdexcept>

extern "C" {
#include <lauxlib.h>
}

#include <cool/defer.hpp>
#include <luapp/table.hpp>

namespace lua
{

table::table(reference ref) noexcept : ref_(std::move(ref)) {}

table::table(std::shared_ptr<lua_State> state, int ref) : table(reference{std::move(state), ref}) {}

auto table::get(std::string_view) const -> reference
{
  return {}; // TODO
}

auto table::set(std::string_view, const value&) const -> void
{
  // TODO:
}

auto table::operator[](std::string_view key) const -> table
{
  const auto state = ref_.state();
  if (!lua_checkstack(state, 2))
    throw std::bad_alloc{};

  ref_.push();
  COOL_DEFER(lua_pop(state, 1));

  lua_pushlstring(state, key.data(), key.size());
  const auto type = lua_gettable(state, -2);

  if (type != LUA_TTABLE)
    throw std::invalid_argument{"t[key] is not a table"};

  const auto index = luaL_ref(state, LUA_REGISTRYINDEX);
  return table(ref_.sstate(), index);
}

} // namespace lua
