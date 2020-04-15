#ifndef LUAPP_TABLE_HPP_INCLUDED
#define LUAPP_TABLE_HPP_INCLUDED

#include <memory>

extern "C" {
#include <lua.h>
}

#include <luapp/reference.hpp>

namespace lua
{

class state;
class value;

namespace detail
{
struct getter;
struct setter;
} // namespace detail

class table
{
  friend class state;
  friend class value;
  friend struct detail::getter;
  friend struct detail::setter;

public:
private:
  explicit table(reference) noexcept;

  auto get(const value&) const -> reference;
  auto set(const value&, const value&) const -> void;

  auto push(std::shared_ptr<state_data>) const -> int;
  auto push(std::shared_ptr<state_data>, lua_State*) const -> int;

  auto as_value() -> const value&;

  reference ref_;
};

} // namespace lua

#endif // LUAPP_TABLE_HPP_INCLUDED
