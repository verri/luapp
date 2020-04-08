#ifndef LUAPP_TYPE_HPP_INCLUDED
#define LUAPP_TYPE_HPP_INCLUDED

#include <any>
#include <string>

extern "C" {
#include <lua.h>
}

#include <luapp/function.hpp>
#include <luapp/table.hpp>
#include <luapp/tuple.hpp>

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

// TODO: how to deal with automatic conversions between integer and floating (that is number)?

// TODO: better names.
template <std::size_t N> constexpr auto nargs = std::integral_constant<std::size_t, N>{};
template <std::size_t N> constexpr auto nrets = std::integral_constant<std::size_t, N>{};

class userdata
{
public:
  userdata() noexcept = default;

  template <typename T> userdata(std::shared_ptr<T> ptr) noexcept : data_(std::move(ptr)) {}

  template <typename T>
  explicit userdata(T value) : data_(std::make_any<std::shared_ptr<T>>(new T(std::move(value))))
  {}

  template <typename T, typename... Args>
  explicit userdata(std::in_place_type_t<T>, Args&&... args)
    : data_(std::make_shared<T>(std::forward<Args>(args)...))
  {}

  userdata(const userdata&) = default;
  userdata(userdata&&) noexcept = default;

  auto operator=(const userdata&) -> userdata& = default;
  auto operator=(userdata&&) noexcept -> userdata& = default;

  template <typename T> auto cast() const noexcept -> std::shared_ptr<T>
  {
    if (const auto* p = std::any_cast<std::shared_ptr<T>>(&data_); p)
      return *p;
    return nullptr;
  }

private:
  std::any data_;
};

} // namespace lua

#endif // LUAPP_TYPE_HPP_INCLUDED
