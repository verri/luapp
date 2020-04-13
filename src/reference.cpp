#include <cassert>
#include <new>

extern "C" {
#include <lauxlib.h>
}

#include <luapp/reference.hpp>
#include <luapp/state.hpp>

namespace lua
{

reference::reference(std::shared_ptr<state_data> state_data, int index)
  : data_(
      new reference_data{state_data, index}, +[](reference_data* d) {
        if (const auto state_data = d->sdata.lock())
          luaL_unref(state_data->state, LUA_REGISTRYINDEX, d->index);
        delete d;
      })
{
  if (!data_)
    throw std::bad_alloc{};
}

auto reference::valid() const noexcept -> bool
{
  return data_ && !data_->sdata.expired() && data_->index != LUA_NOREF;
}

reference::operator bool() const noexcept { return valid(); }

auto reference::push(std::shared_ptr<state_data> s) const -> int
{
  const auto state_data = data_->sdata.lock();
  const auto state = state_data->state;
  assert(state == s->state);

  if (!lua_checkstack(state, 1))
    throw std::bad_alloc{};

  return lua_rawgeti(state, LUA_REGISTRYINDEX, data_->index);
}

auto reference::state() const -> std::shared_ptr<state_data> { return data_->sdata.lock(); }

} // namespace lua
