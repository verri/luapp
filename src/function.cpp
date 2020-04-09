#include <luapp/value.hpp>

#include <luapp/function.hpp>
#include <stdexcept>

namespace lua
{

function::function(std::function<tuple(tuple)> f) noexcept
  : f_(std::make_shared<std::function<tuple(tuple)>>(std::move(f)))
{}

auto function::call(tuple t) const -> tuple { return (*f_)(std::move(t)); }

auto function::push(lua_State*) const -> int { throw std::runtime_error{"not yet implemented"}; }

} // namespace lua
