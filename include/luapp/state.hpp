#ifndef LUAPP_STATE_HPP_INCLUDED
#define LUAPP_STATE_HPP_INCLUDED

#include <memory>
#include <typeindex>

extern "C" {
#include <lua.h>
}

#include <luapp/reference.hpp>
#include <luapp/table.hpp>
#include <luapp/type.hpp>

namespace lua
{

class state
{
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

private:
  std::shared_ptr<lua_State> state_;
  std::shared_ptr<std::unordered_map<std::type_index, reference>> metatables_;
};

} // namespace lua

#endif // LUAPP_STATE_HPP_INCLUDED
