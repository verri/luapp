#ifndef LUAPP_STATE_HPP_INCLUDED
#define LUAPP_STATE_HPP_INCLUDED

extern "C"
{
#include <lua.h>
}

namespace lua
{

class state
{
public:
  enum options : unsigned
  {
    std_libs = 1,
  };

  explicit state(options = std_libs);
  ~state() noexcept;

  state(const state&) = delete;
  auto operator=(const state&) -> state& = delete;

  state(state&&) noexcept;
  auto operator=(state&&) noexcept -> state&;

private:
  lua_State *state_;
};

} // namespace lua

#endif // LUAPP_STATE_HPP_INCLUDED
