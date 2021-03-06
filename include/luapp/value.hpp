#ifndef LUAPP_VALUE_HPP_INCLUDED
#define LUAPP_VALUE_HPP_INCLUDED

#include <memory>
#include <optional>
#include <variant>

#include <luapp/type.hpp>

namespace lua
{

class value
  : private std::variant<nil, floating, integer, boolean, string, function, userdata, table>
{
  using variant = std::variant<nil, floating, integer, boolean, string, function, userdata, table>;

  friend class function;
  friend class userdata;
  friend class table;
  friend class state;

public:
  constexpr value() noexcept = default;

  value(const reference&);

  value(const char* cstr) : value(string(cstr)) {}

  constexpr value(bool v) noexcept : value(boolean(v)) {}

  using variant::variant;

  template <typename T> value(std::shared_ptr<T> v) : value(userdata(std::move(v))) {}

  value(std::optional<floating>) noexcept;
  value(std::optional<integer>) noexcept;
  value(std::optional<boolean>) noexcept;
  value(std::optional<string>) noexcept;
  value(std::optional<function>) noexcept;
  value(std::optional<userdata>) noexcept;
  value(std::optional<table>) noexcept;

  template <typename R, typename... Args> value(R (*f)(Args...)) : value(function(f)) {}

  value(const value&) = default;
  value(value&&) noexcept = default;

  auto operator=(const value&) -> value& = default;
  auto operator=(value&&) noexcept -> value& = default;

  using variant::operator=;

  explicit constexpr operator bool() const
  {
    return is_boolean() ? (bool)std::get<boolean>(*this) : !is_nil();
  }

  template <typename T> constexpr auto is() const noexcept -> bool
  {
    return std::holds_alternative<T>(*this);
  }

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
  constexpr auto get_or(T value) const noexcept(std::is_nothrow_copy_constructible_v<T>) -> T
  {
    if (is<T>())
      return std::get<T>(*this);
    return value;
  }

  constexpr auto get_boolean_or(bool value) const noexcept -> bool
  {
    return get_or<boolean>(value);
  }

  constexpr auto get_number_or(floating value) const noexcept -> floating
  {
    if (is<integer>())
      return std::get<integer>(as_variant());
    return get_or<floating>(value);
  }

  constexpr auto get_floating_or(floating value) const noexcept -> floating
  {
    return get_or<floating>(value);
  }

  constexpr auto get_integer_or(integer value) const noexcept -> integer
  {
    return get_or<integer>(value);
  }

  auto get_string_or(string) const -> string;
  auto get_function_or(function) const noexcept -> function;
  auto get_userdata_or(userdata) const -> userdata;
  auto get_table_or(table) const noexcept -> table;

  constexpr operator std::optional<boolean>() const noexcept
  {
    if (is<boolean>())
      return std::get<boolean>(as_variant());
    return std::nullopt;
  }

  constexpr operator std::optional<floating>() const noexcept
  {
    if (is<floating>())
      return std::get<floating>(as_variant());
    if (is<integer>())
      return std::get<integer>(as_variant());
    return std::nullopt;
  }

  constexpr operator std::optional<integer>() const noexcept
  {
    if (is<integer>())
      return std::get<integer>(as_variant());
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
    return std::visit(
      [](const auto& value) -> std::variant<nil, T...> {
        if constexpr ((std::is_same_v<T, std::decay_t<decltype(value)>> || ...))
          return value;
        else
          return nil{};
      },
      as_variant());
  }

  template <typename T> operator std::shared_ptr<T>() const noexcept
  {
    return std::visit(
      [](const auto& value) -> std::shared_ptr<T> {
        if constexpr (std::is_same_v<std::decay_t<decltype(value)>, userdata>)
          return value.template cast<T>();
        else
          return nullptr;
      },
      as_variant());
  }

  auto operator==(const value&) const -> bool;
  auto operator!=(const value&) const -> bool;

private:
  static auto at(std::shared_ptr<state_data>, int) -> value;
  static auto at(std::shared_ptr<state_data>, lua_State*, int) -> value;
  static auto from_ref(std::shared_ptr<state_data>, const reference&) -> variant;

  static auto checkudata(std::shared_ptr<state_data>, reference) -> value;

  auto push(std::shared_ptr<state_data>) const -> int;
  auto push(std::shared_ptr<state_data>, lua_State*) const -> int;
};

namespace detail
{
struct getter
{
  auto operator()(const table&, const value&) const -> value;
};
struct setter
{
  auto operator()(const table&, const value&, const value&) const -> void;
};
} // namespace detail

auto get(const table&, const value&) -> value;
auto set(const table&, const value&, const value&) -> void;

} // namespace lua

#endif // LUAPP_VALUE_HPP_INCLUDED
