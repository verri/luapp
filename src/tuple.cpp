#include <cassert>

#include <luapp/tuple.hpp>
#include <luapp/value.hpp>

namespace lua
{

tuple::tuple() { values_.emplace_back(); }

tuple::tuple(std::vector<value> values) noexcept : values_(std::move(values))
{
  values_.emplace_back();
}

auto tuple::operator[](std::size_t i) const noexcept -> const value& { return at(i); }

auto tuple::operator[](std::size_t i) -> value&
{
  assert(i < size());
  return values_[i];
}

auto tuple::at(std::size_t i) const noexcept -> const value&
{
  return i < values_.size() ? values_[i] : values_.back();
}

auto tuple::size() const noexcept -> std::size_t { return values_.size() - 1; }

auto tuple::resize(std::size_t s) -> void { values_.resize(s + 1); }

tuple::operator const value&() const noexcept { return values_.front(); }

} // namespace lua
