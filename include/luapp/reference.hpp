#ifndef LUAPP_REFERENCE_HPP_INCLUDED
#define LUAPP_REFERENCE_HPP_INCLUDED

#include <memory>

extern "C" {
#include <lua.h>
}

namespace lua
{

class table;
class value;

class reference
{
  friend class table;
  friend class value;

public:
  reference() noexcept;
  ~reference() noexcept;

  auto valid() const noexcept -> bool;
  operator bool() const noexcept;

private:
  reference(std::shared_ptr<lua_State>, int) noexcept;

  auto push() const -> int;
  auto state() const noexcept -> lua_State*;

  std::shared_ptr<lua_State> state_;
  int ref_;
};

} // namespace lua

#endif // LUAPP_REFERENCE_HPP_INCLUDED
