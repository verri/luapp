#ifndef LUAPP_FUNCTION_HPP_INCLUDED
#define LUAPP_FUNCTION_HPP_INCLUDED

#include <functional>
#include <memory>

#include <luapp/tuple.hpp>
#include <type_traits>

namespace lua
{

class function
{
public:
  function(std::function<tuple(tuple)>) noexcept;

  template <typename R, typename... Args>
  function(R (*f)(Args...)) noexcept
    : function(std::function<tuple(tuple)>([f](tuple t) -> tuple { return t.apply(f); }))
  {
    static_assert(std::is_constructible_v<tuple, R> || std::is_same_v<void, R>);
    static_assert((std::is_convertible_v<value, Args> && ...));
  }

  template <std::size_t N, typename F>
  function(std::integral_constant<std::size_t, N>, F f) noexcept
    : function(std::function<tuple(tuple)>([f = std::move(f)](tuple t) -> tuple {
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

  template <std::size_t N, typename... Args>
  auto operator()(std::integral_constant<std::size_t, N> ret, Args&&... args)
    -> std::array<value, N>
  {
    return call(tuple{std::forward<Args>(args)...}).expand(ret);
  }

private:
  std::shared_ptr<std::function<tuple(tuple)>> f_;
};

} // namespace lua

#endif // LUAPP_FUNCTION_HPP_INCLUDED
