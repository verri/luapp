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
  friend class state;

public:
  userdata(const userdata&) = default;
  userdata(userdata&&) noexcept = default;

  auto operator=(const userdata&) -> userdata& = default;
  auto operator=(userdata&&) noexcept -> userdata& = default;

  template <typename T> auto cast() const noexcept -> std::shared_ptr<T>
  {
    static_assert(!std::is_same_v<T, void>);
    if (const auto* p = std::any_cast<std::shared_ptr<std::decay_t<T>>>(data()); p)
      return *p;
    return nullptr;
  }

  // always consults the metatable.
  auto operator==(const userdata&) const -> bool;
  auto operator!=(const userdata&) const -> bool;

private:
  struct dummy
  {};

  template <typename T>
  explicit userdata(const state& state, T value)
    : userdata(dummy{}, state, std::make_any<std::shared_ptr<T>>(new T(std::move(value))))
  {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
    static_assert(!std::is_same_v<T, void>);
  }

  template <typename T>
  userdata(const state& state, std::shared_ptr<T> ptr)
    : userdata(dummy{}, state, std::any(std::move(ptr)))
  {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
    static_assert(!std::is_same_v<T, void>);
  }

  template <typename T, typename... Args>
  explicit userdata(const state& state, std::in_place_type_t<T>, Args&&... args)
    : userdata(dummy{}, state, std::make_shared<T>(std::forward<Args>(args)...))
  {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
    static_assert(!std::is_same_v<T, void>);
  }

  explicit userdata(reference) noexcept;
  userdata(dummy, const state&, std::any); // dummy parameter to permit shared_ptr<any>

  auto push(std::shared_ptr<state_data>) const -> int;
  auto push(std::shared_ptr<state_data>, lua_State*) const -> int;

  auto data() const -> std::any*;

  static auto create_reference(const state&, std::any) -> reference;

  reference ref_;
};

} // namespace lua

#endif // LUAPP_USERDATA_HPP_INCLUDED
