#include <cassert>
#include <stdexcept>

extern "C" {
#include <lauxlib.h>
}

#include <cool/compose.hpp>
#include <cool/defer.hpp>
#include <luapp/reference.hpp>
#include <luapp/value.hpp>

namespace lua
{

auto value::at(std::shared_ptr<lua_State> sstate, int i) -> value
{
  const auto state = sstate.get();
  const auto type = lua_type(state, i);

  switch (type) {
  case LUA_TNUMBER:
    if (lua_isinteger(state, i))
      return integer{lua_tointeger(state, i)};
    else
      return floating{lua_tonumber(state, i)};
  case LUA_TBOOLEAN:
    return boolean{static_cast<bool>(lua_toboolean(state, i))};
  case LUA_TSTRING:
    return string{lua_tostring(state, i)};
  case LUA_TTABLE:
    lua_pushvalue(state, i);
    return table(reference(std::move(sstate), luaL_ref(state, LUA_REGISTRYINDEX)));
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

template <typename T>
auto from_optional(std::optional<T> v)
  -> std::variant<nil, floating, integer, boolean, string, function, userdata, table>
{
  if (v.has_value())
    return *v;
  return nil{};
}

value::value(std::optional<floating> v) noexcept : variant(from_optional(v)) {}
value::value(std::optional<integer> v) noexcept : variant(from_optional(v)) {}
value::value(std::optional<boolean> v) noexcept : variant(from_optional(v)) {}
value::value(std::optional<string> v) noexcept : variant(from_optional(v)) {}
value::value(std::optional<function> v) noexcept : variant(from_optional(v)) {}
value::value(std::optional<userdata> v) noexcept : variant(from_optional(v)) {}
value::value(std::optional<table> v) noexcept : variant(from_optional(v)) {}

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

auto value::push(lua_State* state) const -> int
{
  if (!lua_checkstack(state, 1))
    throw std::bad_alloc{};

  return std::visit(cool::compose{
                      [state](nil) -> int {
                        lua_pushnil(state);
                        return LUA_TNIL;
                      },
                      [state](floating f) -> int {
                        lua_pushnumber(state, f);
                        return LUA_TNUMBER;
                      },
                      [state](integer i) -> int {
                        lua_pushinteger(state, i);
                        return LUA_TNUMBER;
                      },
                      [state](boolean b) -> int {
                        lua_pushboolean(state, b);
                        return LUA_TBOOLEAN;
                      },
                      [state](const string& s) -> int {
                        lua_pushlstring(state, s.data(), s.size());
                        return LUA_TSTRING;
                      },
                      [state](const auto& other) -> int { return other.push(state); },
                    },
                    as_variant());
}

auto value::operator[](const value& key) const -> value
{
  // XXX: should userdata use operator[] too?
  if (!is_table())
    throw std::runtime_error{"indexing an invalid value"};

  return std::get<table>(as_variant()).get(key);
}

} // namespace lua
