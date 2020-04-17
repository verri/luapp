#ifndef LUAPP_STATE_HPP_INCLUDED
#define LUAPP_STATE_HPP_INCLUDED

#include <initializer_list>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

extern "C" {
#include <lua.h>
}

#include <luapp/reference.hpp>
#include <luapp/table.hpp>
#include <luapp/tuple.hpp>
#include <luapp/userdata.hpp>

namespace lua
{

struct state_data
{
  lua_State* state;
  std::unordered_map<std::type_index, table> metatables;
};

class userdata;

class state
{
  friend class userdata;

public:
  enum options : unsigned {
    std_libs = 1,
  };

  explicit state(options = std_libs);
  ~state() noexcept = default;

  state(const state&) noexcept = default;
  state(state&&) noexcept = default;

  auto operator=(const state&) noexcept -> state& = default;
  auto operator=(state&&) noexcept -> state& = default;

  auto global_table() const -> table;

  auto do_string(const char*) const -> tuple;

  template <std::size_t N>
  auto do_string(std::integral_constant<std::size_t, N> nrets, const char* code) const
  {
    return do_string(code).expand(nrets);
  }

  auto create_table() const -> table;

  template <typename T> auto create_userdata(std::shared_ptr<T> ptr) const
  {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
    static_assert(!std::is_same_v<T, void>);
    return userdata(*this, typeid(T), std::move(ptr));
  }

  template <typename T> auto create_userdata(T value) const
  {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
    static_assert(!std::is_same_v<T, void>);
    return userdata(*this, typeid(T), std::make_shared<T>(std::move(value)));
  }

  template <typename T, typename... Args>
  auto create_userdata(std::in_place_type_t<T>, Args&&... args) const
  {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
    static_assert(!std::is_same_v<T, void>);
    return userdata(*this, typeid(T), std::make_shared<T>(std::forward<Args>(args)...));
  }

  auto register_metatable(std::type_index, std::initializer_list<std::pair<value, value>>) const
    -> void;

private:
  auto get_metatable(std::type_index) const -> const table&;

  std::shared_ptr<state_data> data_;
};

} // namespace lua

#endif // LUAPP_STATE_HPP_INCLUDED
