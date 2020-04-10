#ifndef LUAPP_TYPE_HPP_INCLUDED
#define LUAPP_TYPE_HPP_INCLUDED

#include <any>
#include <string>
#include <type_traits>

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

template <std::size_t N> constexpr auto args = std::integral_constant<std::size_t, N>{};
template <std::size_t N> constexpr auto returns = std::integral_constant<std::size_t, N>{};

class userdata
{
  friend class value;

public:
  userdata() noexcept = default;

  template <typename T> userdata(std::shared_ptr<T> ptr) noexcept : data_(std::move(ptr))
  {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
  }

  template <typename T>
  explicit userdata(T value) : data_(std::make_any<std::shared_ptr<T>>(new T(std::move(value))))
  {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
  }

  template <typename T, typename... Args>
  explicit userdata(std::in_place_type_t<T>, Args&&... args)
    : data_(std::make_shared<T>(std::forward<Args>(args)...))
  {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
  }

  userdata(const userdata&) = default;
  userdata(userdata&&) noexcept = default;

  auto operator=(const userdata&) -> userdata& = default;
  auto operator=(userdata&&) noexcept -> userdata& = default;

  template <typename T> auto cast() const noexcept -> std::shared_ptr<T>
  {
    if (const auto* p = std::any_cast<std::shared_ptr<std::decay_t<T>>>(&data_); p)
      return *p;
    return nullptr;
  }

private:
  auto push(lua_State*) const -> int;

  std::any data_;
};

} // namespace lua

#endif // LUAPP_TYPE_HPP_INCLUDED
