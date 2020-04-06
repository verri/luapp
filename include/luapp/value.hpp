#ifndef LUAPP_VALUE_HPP_INCLUDED
#define LUAPP_VALUE_HPP_INCLUDED

#include <type_traits>
extern "C"
{
#include <lua.h>
}

#include <luapp/type.hpp>

#include <optional>
#include <variant>

namespace lua
{

class value : private std::variant<nil, boolean, number, integer, string, function, userdata, table>
{
  using variant = std::variant<nil, boolean, number, integer, string, function, userdata, table>;

public:
  constexpr value() noexcept = default;

  value(const value&) = default;
  value(value&&) noexcept = default;

  auto operator=(const value&) -> value& = default;
  auto operator=(value&&) noexcept -> value& = default;

  using variant::variant;
  using variant::operator=;

  constexpr operator bool() const
  {
    return is_boolean() ? std::get<boolean>(*this) : !is_nil();
  }

  template <typename T>
  constexpr auto is() const noexcept -> bool { return std::holds_alternative<T>(*this); }

  constexpr auto is_nil() const noexcept -> bool { return is<nil>(); }
  constexpr auto is_boolean() const noexcept -> bool { return is<boolean>(); }
  constexpr auto is_number() const noexcept -> bool { return is<number>(); }
  constexpr auto is_integer() const noexcept -> bool { return is<integer>(); }
  constexpr auto is_string() const noexcept -> bool { return is<string>(); }
  constexpr auto is_function() const noexcept -> bool { return is<function>(); }
  constexpr auto is_userdata() const noexcept -> bool { return is<userdata>(); }
  constexpr auto is_table() const noexcept -> bool { return is<table>(); }

  constexpr auto as_variant() const noexcept -> const variant& { return *this; }
  constexpr auto as_variant() noexcept -> variant& { return *this; }

  template <typename T>
  constexpr auto get_or(T value) const noexcept(std::is_nothrow_copy_constructible_v<T>) -> T {
    if (is<T>())
      return std::get<T>(*this);
    return value;
  }

  constexpr auto get_boolean(bool value) const noexcept -> bool { return get_or<boolean>(value); }
  constexpr auto get_number(number value) const noexcept -> number { return get_or<number>(value); }
  constexpr auto get_integer(integer value) const noexcept -> integer { return get_or<integer>(value); }

  auto get_string(string) const -> string;
  auto get_function(function) const -> function;
  auto get_userdata(userdata) const -> userdata;
  auto get_table(table) const -> table;
};

} // namespace lua

#endif // LUAPP_VALUE_HPP_INCLUDED
