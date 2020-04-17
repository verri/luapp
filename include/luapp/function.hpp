#ifndef LUAPP_FUNCTION_HPP_INCLUDED
#define LUAPP_FUNCTION_HPP_INCLUDED

#include <functional>
#include <memory>
#include <type_traits>
#include <variant>

extern "C" {
#include <lua.h>
}

#include <luapp/reference.hpp>
#include <luapp/tuple.hpp>

namespace lua
{

struct state_data;

class function
{
  friend class value;

public:
  function(std::function<tuple(tuple)>) noexcept;

  template <typename R, typename... Args>
  function(R (*f)(Args...))
    : function(std::function<tuple(tuple)>([f](tuple t) -> tuple { return t.apply(f); }))
  {
    static_assert(std::is_constructible_v<tuple, R> || std::is_same_v<void, R>);
    static_assert((std::is_convertible_v<value, Args> && ...));
  }

  template <std::size_t N, typename F>
  function(std::integral_constant<std::size_t, N>, F f)
    : function(std::function<tuple(tuple)>([f = std::move(f)](tuple t) -> tuple {
        return t.apply(std::integral_constant<std::size_t, N>{}, f);
      }))
  {
    using R = decltype(std::apply(std::declval<F>(), std::declval<std::array<value, N>>()));
    static_assert(std::is_constructible_v<tuple, R> || std::is_same_v<void, R>);
  }

  function(const function&) = default;
  function(function&&) noexcept = default;

  auto operator=(const function&) -> function& = default;
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

  // because of the complexity of dealing with closures and stuff, they are always
  // different from each other.  This behavior might change in future versions.
  auto operator==(const function&) const -> bool;
  auto operator!=(const function&) const -> bool;

private:
  function(reference) noexcept;

  auto push(std::shared_ptr<state_data>) const -> int;
  auto push(std::shared_ptr<state_data>, lua_State*) const -> int;

  std::variant<std::shared_ptr<std::function<tuple(tuple)>>, reference> f_;
};

} // namespace lua

#endif // LUAPP_FUNCTION_HPP_INCLUDED
