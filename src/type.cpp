#include <luapp/type.hpp>

namespace lua
{

auto userdata::push(lua_State*) const -> int { throw std::runtime_error{"not yet implemented"}; }

} // namespace lua
