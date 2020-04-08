#include <cassert>

extern "C" {
#include <lauxlib.h>
}

#include <cool/defer.hpp>
#include <luapp/reference.hpp>
#include <luapp/value.hpp>

namespace lua
{

auto value::from_ref(const reference& ref) -> variant
{
  if (!ref.valid())
    return nil{};

  const auto state = ref.state();

  const auto type = ref.push();
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
    return table(ref);
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

value::value(const reference& ref) : variant{from_ref(ref)} {}

auto value::get_string_or(string value) const -> string { return get_or<string>(std::move(value)); }

auto value::get_function_or(function value) const noexcept -> function
{
  return get_or<function>(std::move(value));
}

auto value::get_userdata_or(userdata value) const -> userdata
{
  return get_or<userdata>(std::move(value));
}

auto value::get_table_or(table value) const noexcept -> table
{
  return get_or<table>(std::move(value));
}

value::operator std::optional<string>() const
{
  if (is<string>())
    return std::get<string>(*this);
  return std::nullopt;
}

value::operator std::optional<function>() const noexcept
{
  if (is<function>())
    return std::get<function>(*this);
  return std::nullopt;
}

value::operator std::optional<userdata>() const
{
  if (is<userdata>())
    return std::get<userdata>(*this);
  return std::nullopt;
}

value::operator std::optional<table>() const noexcept
{
  if (is<table>())
    return std::get<table>(*this);
  return std::nullopt;
}

} // namespace lua
