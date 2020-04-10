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
class state;

class reference
{
  friend class table;
  friend class value;
  friend class state;

public:
  reference() noexcept;

  auto valid() const noexcept -> bool;
  operator bool() const noexcept;

private:
  explicit reference(std::shared_ptr<lua_State>);

  auto push() const -> int;
  auto state() const noexcept -> lua_State*;
  auto sstate() const noexcept -> std::shared_ptr<lua_State>;

  std::shared_ptr<lua_State> state_;
  int ref_;

  std::shared_ptr<void> deleter_;
};

} // namespace lua

#endif // LUAPP_REFERENCE_HPP_INCLUDED
