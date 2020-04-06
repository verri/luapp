#include <luapp/value.hpp>

namespace lua
{

auto value::get_string(string value) const -> string
{
  return get_or<string>(std::move(value));
}

auto value::get_function(function value) const -> function
{
  return get_or<function>(std::move(value));
}

auto value::get_userdata(userdata value) const -> userdata
{
  return get_or<userdata>(std::move(value));
}

auto value::get_table(table value) const -> table
{
  return get_or<table>(std::move(value));
}

} // namespace lua
