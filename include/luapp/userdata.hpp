#ifndef LUAPP_USERDATA_HPP_INCLUDED
#define LUAPP_USERDATA_HPP_INCLUDED

#include <any>
#include <memory>
#include <optional>
#include <typeindex>

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

  template <typename T> operator std::shared_ptr<T>() const noexcept { return cast<T>(); }

  // always consults the metatable.
  auto operator==(const userdata&) const -> bool;
  auto operator!=(const userdata&) const -> bool;

private:
  struct dummy
  {};

  explicit userdata(reference) noexcept;
  userdata(const state&, std::type_index, std::any);

  auto push(std::shared_ptr<state_data>) const -> int;
  auto push(std::shared_ptr<state_data>, lua_State*) const -> int;

  auto data() const -> std::any*;

  static auto create_reference(const state&, std::type_index, std::any) -> reference;

  reference ref_;
};

} // namespace lua

#endif // LUAPP_USERDATA_HPP_INCLUDED
