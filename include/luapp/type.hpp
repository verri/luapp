#ifndef LUAPP_TYPE_HPP_INCLUDED
#define LUAPP_TYPE_HPP_INCLUDED

extern "C"
{
#include <lua.h>
}

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>
#include <utility>

namespace lua
{

class value;

struct nil {};

class boolean
{
public:
  constexpr boolean() noexcept = default;
  constexpr boolean(bool value) noexcept : value_(value) {}
  constexpr operator bool() const noexcept { return value_; }
private:
  bool value_ = false;
};

using floating = lua_Number;
using integer = lua_Integer;
using string = std::string;

// TODO: how to deal with automatic conversions between integer and floating (that is number)

template <std::size_t N>
constexpr auto nargs = std::integral_constant<std::size_t, N>{};

class tuple
{
public:
  tuple();

  explicit tuple(std::vector<value>) noexcept;

  template <typename... Args>
  explicit tuple(Args&&... args) : tuple(std::vector<value>{std::forward<Args>(args)...})
  {
    static_assert((std::is_constructible_v<value, Args> && ...));
  }

  template <typename R, typename... Args>
  auto apply(R(*f)(Args...)) const -> tuple {
    static_assert(std::is_constructible_v<tuple, R> || std::is_same_v<void, R>);
    static_assert((std::is_convertible_v<value, Args> && ...));
    return apply(f, std::make_index_sequence<sizeof...(Args)>());
  }

  template <std::size_t N, typename F>
  auto apply(std::integral_constant<std::size_t, N>, const F& f) const -> tuple
  {
    using R = decltype(std::apply(std::declval<F>(), std::declval<std::array<value, N>>()));
    static_assert(std::is_constructible_v<tuple, R>);
    return apply(f, std::make_index_sequence<N>());
  }

  auto operator[](std::size_t) const noexcept -> const value&;
  auto operator[](std::size_t) -> value&;

  auto at(std::size_t) const noexcept -> const value&;

  template <std::size_t N>
  auto expand(std::integral_constant<std::size_t, N>) const -> std::array<value, N>
  {
    return expand(std::make_index_sequence<N>());
  }

  auto size() const noexcept -> std::size_t;

  operator const value&() const noexcept;

private:
  template <typename F, std::size_t... I>
  auto apply(const F& f, std::index_sequence<I...>) const -> tuple {
    if constexpr (std::is_same_v<tuple, decltype(f(at(I)...))>)
      return f(at(I)...);
    else if constexpr (std::is_same_v<void, decltype(f(at(I)...))>)
      return f(at(I)...), tuple{};
    else
      return tuple{f(at(I)...)}; // deals with explicit instatiation.
  }

  template <std::size_t... I>
  auto expand(std::index_sequence<I...>) const -> std::array<value, sizeof...(I)>
  {
    return {{at(I)...}};
  }

  std::vector<value> values_; // last is always nil
};

class function
{
public:
  function(std::function<tuple(tuple)>) noexcept;

  template <typename R, typename... Args>
  function(R(*f)(Args...)) noexcept :
    function(std::function<tuple(tuple)>([f](tuple t) -> tuple {
      return t.apply(f);
    }))
  {
    static_assert(std::is_constructible_v<tuple, R> || std::is_same_v<void, R>);
    static_assert((std::is_convertible_v<value, Args> && ...));
  }

  template <std::size_t N, typename F>
  function(std::integral_constant<std::size_t, N>, F f) noexcept :
    function(std::function<tuple(tuple)>([f = std::move(f)](tuple t) -> tuple {
      return t.apply(std::integral_constant<std::size_t, N>{}, f);
    }))
  {
    using R = decltype(std::apply(std::declval<F>(), std::declval<std::array<value, N>>()));
    static_assert(std::is_constructible_v<tuple, R> || std::is_same_v<void, R>);
  }

  function(const function&) noexcept = default;
  function(function&&) noexcept = default;

  auto operator=(const function&) noexcept -> function& = default;
  auto operator=(function&&) noexcept -> function& = default;

  auto call(tuple) const -> tuple;

  template <typename... Args> auto operator()(Args&&... args) -> tuple
  {
    return call(tuple{std::forward<Args>(args)...});
  }

private:
  std::shared_ptr<std::function<tuple(tuple)>> f_;
};

class userdata
{
public:
  userdata() noexcept = default;

  template <typename T>
  userdata(std::shared_ptr<T> ptr) noexcept : data_(std::move(ptr)) {}

  template <typename T>
  explicit userdata(T value) : data_(std::make_any<std::shared_ptr<T>>(new T(std::move(value)))) {}

  template <typename T, typename... Args>
  explicit userdata(std::in_place_type_t<T>, Args&&... args) :
    data_(std::make_shared<T>(std::forward<Args>(args)...)) {}

  userdata(const userdata&) = default;
  userdata(userdata&&) noexcept = default;

  auto operator=(const userdata&) -> userdata& = default;
  auto operator=(userdata&&) noexcept -> userdata& = default;

  template <typename T> auto cast() const -> std::shared_ptr<T>
  {
    if (const auto* p = std::any_cast<std::shared_ptr<T>>(&data_); p)
      return *p;
    return nullptr;
  }

private:
  std::any data_;
};

class table
{
public:

private:

};

} // namespace lua

#endif // LUAPP_TYPE_HPP_INCLUDED
