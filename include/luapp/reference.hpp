#ifndef LUAPP_REFERENCE_HPP_INCLUDED
#define LUAPP_REFERENCE_HPP_INCLUDED

#include <memory>

extern "C" {
#include <lua.h>
}

namespace lua
{

struct state_data;

struct reference_data
{
  std::weak_ptr<state_data> sdata;
  int index;
};

class reference
{
  friend class table;
  friend class value;
  friend class state;
  friend class userdata;
  friend class function;

public:
  reference() noexcept = default;

  reference(const reference&) noexcept = default;
  reference(reference&) noexcept = default;

  auto operator=(const reference&) noexcept -> reference& = default;
  auto operator=(reference&) noexcept -> reference& = default;

  auto valid() const noexcept -> bool;
  operator bool() const noexcept;

private:
  reference(std::shared_ptr<state_data>, int);

  auto push(std::shared_ptr<state_data>) const -> int;
  auto push(std::shared_ptr<state_data>, lua_State* state) const -> int;

  auto state() const -> std::shared_ptr<state_data>;

  std::shared_ptr<reference_data> data_;
};

} // namespace lua

#endif // LUAPP_REFERENCE_HPP_INCLUDED
