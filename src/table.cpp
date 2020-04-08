#include <luapp/table.hpp>

namespace lua
{

table::table(reference ref) noexcept : ref_(std::move(ref)) {}

} // namespace lua
