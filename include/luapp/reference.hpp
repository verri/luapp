#ifndef LUAPP_REFERENCE_HPP_INCLUDED
#define LUAPP_REFERENCE_HPP_INCLUDED

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
  explicit constexpr reference(lua_State *state, int index) noexcept : state_{state}, index_{index} {}

  lua_State *state_;
  int index_;
};

} // namespace lua

#endif // LUAPP_REFERENCE_HPP_INCLUDED
