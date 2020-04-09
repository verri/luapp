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

  auto get(const value&) const -> reference;
  auto set(const value&, const value&) const -> void;

private:
  explicit table(reference) noexcept;
  table(std::shared_ptr<lua_State>, int);

  auto push() const -> int;
  auto push(lua_State*) const -> int;

  reference ref_;
};

} // namespace lua

#endif // LUAPP_TABLE_HPP_INCLUDED
