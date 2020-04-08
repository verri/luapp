#ifndef LUAPP_TABLE_HPP_INCLUDED
#define LUAPP_TABLE_HPP_INCLUDED

#include <memory>
#include <optional>

extern "C" {
#include <lua.h>
}

#include <luapp/reference.hpp>

namespace lua
{

class state;
class value;

class table
{
  friend class state;
  friend class value;

public:
  auto get(std::string_view) const -> reference;
  auto set(std::string_view, const value&) const -> void;

  auto operator[](std::string_view) const -> table;

private:
  explicit table(reference) noexcept;
  table(std::shared_ptr<lua_State>, int);

  reference ref_;
};

} // namespace lua

#endif // LUAPP_TABLE_HPP_INCLUDED
