#ifndef LUAPP_TUPLE_HPP_INCLUDED
#define LUAPP_TUPLE_HPP_INCLUDED

#include <array>
#include <tuple>
#include <vector>

namespace lua
{

class value;

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

  template <typename R, typename... Args> auto apply(R (*f)(Args...)) const -> tuple
  {
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
  auto resize(std::size_t) -> void;

  operator const value&() const noexcept;

private:
  template <typename F, std::size_t... I>
  auto apply(const F& f, std::index_sequence<I...>) const -> tuple
  {
    if constexpr (std::is_same_v<tuple, decltype(f(at(I)...))>)
      return f(at(I)...);
    else if constexpr (std::is_same_v<void, decltype(f(at(I)...))>)
      return f(at(I)...), tuple{};
    else
      return tuple{f(at(I)...)}; // deals with explicit constructor.
  }

  template <std::size_t... I>
  auto expand(std::index_sequence<I...>) const -> std::array<value, sizeof...(I)>
  {
    return {{at(I)...}};
  }

  std::vector<value> values_; // last is always nil
};

} // namespace lua

#endif // LUAPP_TUPLE_HPP_INCLUDED
