#include <new>
#include <utility>

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

#include <luapp/state.hpp>

namespace lua
{

state::state(options opt) : state_{luaL_newstate()}
{
  if (!state_)
    throw std::bad_alloc{};

  if (opt & options::std_libs)
    luaL_openlibs(state_);
}

state::~state() noexcept
{
  if (state_)
    lua_close(state_);
}

state::state(state&& other) noexcept : state_{std::exchange(other.state_, nullptr)} {}

auto state::operator=(state&& other) noexcept -> state&
{
  std::swap(state_, other.state_);
  return *this;
}

} // namespace lua
