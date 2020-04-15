#include "cool/indices.hpp"
#include <functional>
#include <luapp/value.hpp>

#include <luapp/function.hpp>
#include <luapp/state.hpp>
#include <memory>
#include <stdexcept>

namespace lua
{

function::function(std::function<tuple(tuple)> f) noexcept
  : f_(std::make_shared<std::function<tuple(tuple)>>(std::move(f)))
{}

auto function::call(tuple t) const -> tuple { return (*f_)(std::move(t)); }

auto function::push(std::shared_ptr<state_data> sdata) const -> int
{
  const auto state = sdata->state;
  return push(std::move(sdata), state);
}

auto function::push(std::shared_ptr<state_data> sdata, lua_State* state) const -> int
{
  if (ref_)
    return ref_->push(std::move(sdata), state);
  throw std::runtime_error{"not yet implemented"};

  using fn_t = std::shared_ptr<std::function<tuple(tuple)>>;

  {
    auto* p = lua_newuserdata(state, sizeof(fn_t));
    new (p) fn_t(f_);
  }

  sdata->metatables.at(typeid(fn_t)).push(sdata, state);
  lua_setmetatable(state, -2);

  {
    auto* p = lua_newuserdata(state, sizeof(std::shared_ptr<state_data>));
    new (p) std::shared_ptr<state_data>(sdata);
  }

  lua_pushcclosure(
    state,
    +[](lua_State* state) -> int {
      const auto f = *reinterpret_cast<fn_t*>(lua_touserdata(state, lua_upvalueindex(1)));
      const auto sdata =
        *reinterpret_cast<std::shared_ptr<state_data>*>(lua_touserdata(state, lua_upvalueindex(2)));

      const auto nargs = lua_gettop(state);

      tuple args;
      args.resize(nargs);

      for (const auto i : cool::closed_indices(1, nargs))
        args[i - 1] = value::at(sdata, state, i);

      const auto ret = (*f)(args);
      for (const auto i : cool::indices(ret.size()))
        ret[i].push(sdata, state);

      return ret.size();
    },
    2);

  return LUA_TFUNCTION;
}

} // namespace lua
