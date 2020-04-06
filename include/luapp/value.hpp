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

class value : private std::variant<nil, floating, integer, boolean, string, function, userdata, table>
{
  using variant = std::variant<nil, floating, integer, boolean, string, function, userdata, table>;

public:
  constexpr value() noexcept = default;

  value(const char* cstr) : value(string(cstr)) {}

  constexpr value(bool v) noexcept : value(boolean(v)) {}

  value(const value&) = default;
  value(value&&) noexcept = default;

  auto operator=(const value&) -> value& = default;
  auto operator=(value&&) noexcept -> value& = default;

  using variant::variant;
  using variant::operator=;

  explicit constexpr operator bool() const
  {
    return is_boolean() ? (bool)std::get<boolean>(*this) : !is_nil();
  }

  template <typename T>
  constexpr auto is() const noexcept -> bool { return std::holds_alternative<T>(*this); }

  constexpr auto is_nil() const noexcept -> bool { return is<nil>(); }
  constexpr auto is_boolean() const noexcept -> bool { return is<boolean>(); }
  constexpr auto is_floating() const noexcept -> bool { return is<floating>(); }
  constexpr auto is_integer() const noexcept -> bool { return is<integer>(); }
  constexpr auto is_number() const noexcept -> bool { return is<floating>() || is<integer>(); }
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

  constexpr auto get_boolean_or(bool value) const noexcept -> bool { return get_or<boolean>(value); }
  constexpr auto get_floating_or(floating value) const noexcept -> floating { return get_or<floating>(value); }
  constexpr auto get_integer_or(integer value) const noexcept -> integer { return get_or<integer>(value); }

  auto get_string_or(string) const -> string;
  auto get_function_or(function) const noexcept -> function;
  auto get_userdata_or(userdata) const -> userdata;
  auto get_table_or(table) const noexcept -> table;

  constexpr operator std::optional<boolean>() const noexcept
  {
    if (is<boolean>())
      return std::get<boolean>(*this);
    return std::nullopt;
  }

  constexpr operator std::optional<floating>() const noexcept
  {
    if (is<floating>())
      return std::get<floating>(*this);
    return std::nullopt;
  }

  constexpr operator std::optional<integer>() const noexcept
  {
    if (is<integer>())
      return std::get<integer>(*this);
    return std::nullopt;
  }

  operator std::optional<string>() const;
  operator std::optional<function>() const noexcept;
  operator std::optional<userdata>() const;
  operator std::optional<table>() const noexcept;

  template <typename... T>
  constexpr operator std::variant<nil, T...>() const
    noexcept((std::is_nothrow_copy_constructible_v<T> && ...))
  {
    return std::visit([](const auto& value) -> std::variant<nil, T...> {
      if constexpr ((std::is_same_v<T, std::decay_t<decltype(value)>> || ...))
        return value;
      else
        return nil{};
    }, as_variant());
  }
};

} // namespace lua

#endif // LUAPP_VALUE_HPP_INCLUDED
