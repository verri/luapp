#ifndef LUAPP_STATE_HPP_INCLUDED
#define LUAPP_STATE_HPP_INCLUDED

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
  std::unordered_map<std::type_index, reference> metatables;
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

  template <typename... Args>
  auto create_userdata(Args&&... args) const -> userdata
  {
    return userdata(*this, std::forward<Args>(args)...);
  }

private:
  auto get_metatable(std::type_index) const -> const reference&;

  std::shared_ptr<state_data> data_;
};

} // namespace lua

#endif // LUAPP_STATE_HPP_INCLUDED
