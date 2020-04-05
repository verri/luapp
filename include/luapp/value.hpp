#ifndef LUAPP_VALUE_HPP_INCLUDED
#define LUAPP_VALUE_HPP_INCLUDED

extern "C"
{
#include <lua.h>
}

#include <luapp/type.hpp>

#include <variant>

namespace lua
{

class value
{
  using variant = std::variant<nil, boolean, number, integer, string, table>;

public:
  constexpr value() noexcept = default;

  value(const value&) = default;
  value(value&&) noexcept = default;

  auto operator=(const value&) -> value& = default;
  auto operator=(value&&) noexcept -> value& = default;

  template <typename T>
  constexpr auto is() const -> bool { return std::holds_alternative<T>(data_); }

private:
  variant data_;
};

} // namespace lua

#endif // LUAPP_VALUE_HPP_INCLUDED
