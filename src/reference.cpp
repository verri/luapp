#include <cassert>
#include <new>

extern "C" {
#include <lauxlib.h>
}

#include <luapp/reference.hpp>

namespace lua
{

reference::reference() noexcept : ref_{LUA_NOREF} {}

reference::reference(std::shared_ptr<lua_State> state)
  : state_{std::move(state)},
    ref_{luaL_ref(state_.get(), LUA_REGISTRYINDEX)},
    deleter_(nullptr,
             [state = state_.get(), r = ref_](auto) { luaL_unref(state, LUA_REGISTRYINDEX, r); })
{}

auto reference::valid() const noexcept -> bool { return state_ && ref_ != LUA_NOREF; }

reference::operator bool() const noexcept { return valid(); }

auto reference::push(lua_State* state) const -> int
{
  assert(state_.get() == state);

  if (!lua_checkstack(state_.get(), 1))
    throw std::bad_alloc{};

  return lua_rawgeti(state_.get(), LUA_REGISTRYINDEX, ref_);
}

auto reference::state() const noexcept -> lua_State* { return state_.get(); }

auto reference::sstate() const noexcept -> std::shared_ptr<lua_State> { return state_; }

} // namespace lua
