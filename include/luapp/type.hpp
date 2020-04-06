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

using boolean = int;
using number = lua_Number;
using integer = lua_Integer;
using string = std::string;

class tuple
{
public:
  tuple() = default;

  explicit tuple(std::vector<value>) noexcept;

  template <typename... Args>
  explicit tuple(Args&&... args) : tuple(std::vector<value>{std::forward<Args>(args)...})
  {
    static_assert((std::is_constructible_v<value, Args> && ...));
  }

  template <typename R, typename... Args>
  auto apply(R(*f)(Args...)) const -> tuple {
    static_assert(std::is_constructible_v<tuple, R>);
    static_assert((std::is_same_v<value, Args> && ...));
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

private:
  template <typename F, std::size_t... I>
  auto apply(const F& f, std::index_sequence<I...>) const -> tuple {
    return f(at(I)...);
  }

  std::vector<value> values_; // last is always nil
};

template <std::size_t N>
struct nargs_t : public std::integral_constant<std::size_t, N> {};

template <std::size_t N>
constexpr auto nargs = nargs_t<N>{};

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
    static_assert(std::is_constructible_v<tuple, R>);
    static_assert((std::is_same_v<value, Args> && ...));
  }

  template <std::size_t N, typename F>
  function(std::integral_constant<std::size_t, N>, F f) noexcept :
    function(std::function<tuple(tuple)>([f = std::move(f)](tuple t) -> tuple {
      return t.apply(std::integral_constant<std::size_t, N>{}, f);
    }))
  {
    using R = decltype(std::apply(std::declval<F>(), std::declval<std::array<value, N>>()));
    static_assert(std::is_constructible_v<tuple, R>);
  }

  auto operator()(tuple) const -> tuple;

  template <typename... Args> auto operator()(Args&&... args) -> tuple
  {
    return (*this)(tuple{std::forward<Args>(args)...});
  }

private:
  std::function<tuple(tuple)> f_;
};

class userdata
{
public:
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
