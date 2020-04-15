#ifndef LUAPP_USERDATA_HPP_INCLUDED
#define LUAPP_USERDATA_HPP_INCLUDED

#include <any>
#include <memory>
#include <optional>

#include <luapp/table.hpp>

namespace lua
{

class userdata
{
  friend class value;

public:
  userdata() noexcept = default;

  template <typename T>
  userdata(const state& state, std::shared_ptr<T> ptr) noexcept
    : userdata(state, std::any(std::move(ptr)))
  {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
    static_assert(!std::is_same_v<T, void>);
  }

  template <typename T>
  explicit userdata(const state& state, T value)
    : userdata(state, std::make_any<std::shared_ptr<T>>(new T(std::move(value))))
  {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
    static_assert(!std::is_same_v<T, void>);
  }

  template <typename T, typename... Args>
  explicit userdata(const state& state, std::in_place_type_t<T>, Args&&... args)
    : userdata(state, std::make_shared<T>(std::forward<Args>(args)...))
  {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
    static_assert(!std::is_same_v<T, void>);
  }

  userdata(const userdata&) = default;
  userdata(userdata&&) noexcept = default;

  auto operator=(const userdata&) -> userdata& = default;
  auto operator=(userdata&&) noexcept -> userdata& = default;

  template <typename T> auto cast() const noexcept -> std::shared_ptr<T>
  {
    static_assert(!std::is_same_v<T, void>);
    if (const auto* p = std::any_cast<std::shared_ptr<std::decay_t<T>>>(&data_); p)
      return *p;
    return nullptr;
  }

private:
  userdata(const state&, std::any) noexcept;
  userdata(std::any, const reference&) noexcept;

  auto push(std::shared_ptr<state_data>) const -> int;
  auto push(std::shared_ptr<state_data>, lua_State*) const -> int;

  std::any data_;
  reference metatable_;
};

} // namespace lua

#endif // LUAPP_USERDATA_HPP_INCLUDED
