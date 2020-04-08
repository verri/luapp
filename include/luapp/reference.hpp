#ifndef LUAPP_REFERENCE_HPP_INCLUDED
#define LUAPP_REFERENCE_HPP_INCLUDED

#include <memory>
#include <optional>

extern "C" {
#include <lua.h>
}

#include <luapp/value.hpp>

namespace lua
{

class state;

class reference
{
  friend class state;

public:
  reference() noexcept;
  ~reference() noexcept;

  operator value() const;

private:
  reference(std::weak_ptr<lua_State>, int) noexcept;

  std::weak_ptr<lua_State> state_;
  int index_;
};

} // namespace lua

#endif // LUAPP_REFERENCE_HPP_INCLUDED
