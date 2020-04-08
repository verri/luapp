#include <cassert>
#include <new>

extern "C" {
#include <lauxlib.h>
}

#include <luapp/reference.hpp>

namespace lua
{

reference::reference() noexcept : ref_{LUA_NOREF} {}

reference::~reference() noexcept
{
  if (state_)
    luaL_unref(state_.get(), LUA_REGISTRYINDEX, ref_);
}

reference::reference(std::shared_ptr<lua_State> state, int ref) noexcept
  : state_{std::move(state)}, ref_{ref}
{}

auto reference::valid() const noexcept -> bool { return state_ && ref_ != LUA_NOREF; }

reference::operator bool() const noexcept { return valid(); }

auto reference::push() const -> int
{
  assert(state_);

  if (!lua_checkstack(state_.get(), 1))
    throw std::bad_alloc{};

  return lua_rawgeti(state_.get(), LUA_REGISTRYINDEX, ref_);
}

auto reference::state() const noexcept -> lua_State* { return state_.get(); }

} // namespace lua
