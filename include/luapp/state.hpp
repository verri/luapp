#ifndef LUAPP_STATE_HPP_INCLUDED
#define LUAPP_STATE_HPP_INCLUDED

#include <memory>

extern "C" {
#include <lua.h>
}

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

private:
  std::shared_ptr<lua_State> state_;
};

} // namespace lua

#endif // LUAPP_STATE_HPP_INCLUDED
