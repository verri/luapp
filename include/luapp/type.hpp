#ifndef LUAPP_TYPE_HPP_INCLUDED
#define LUAPP_TYPE_HPP_INCLUDED

#include <string>
#include <type_traits>

extern "C" {
#include <lua.h>
}

#include <luapp/function.hpp>
#include <luapp/table.hpp>
#include <luapp/tuple.hpp>
#include <luapp/userdata.hpp>

namespace lua
{

class value;

struct nil
{};

class boolean
{
public:
  constexpr boolean() noexcept = default;
  constexpr boolean(bool value) noexcept : value_(value) {}
  constexpr operator bool() const noexcept { return value_; }

  constexpr boolean(const boolean&) noexcept = default;
  constexpr boolean(boolean&&) noexcept = default;

  constexpr auto operator=(const boolean&) noexcept -> boolean& = default;
  constexpr auto operator=(boolean&&) noexcept -> boolean& = default;

private:
  bool value_ = false;
};

using floating = lua_Number;
using integer = lua_Integer;
using string = std::string;

template <std::size_t N> constexpr auto args = std::integral_constant<std::size_t, N>{};
template <std::size_t N> constexpr auto returns = std::integral_constant<std::size_t, N>{};

} // namespace lua

#endif // LUAPP_TYPE_HPP_INCLUDED
