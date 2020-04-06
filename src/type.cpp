#include <luapp/type.hpp>

#include <luapp/value.hpp>
#include <stdexcept>

namespace lua
{

tuple::tuple(std::vector<value> values) noexcept : values_(std::move(values))
{
  values_.emplace_back();
}

auto tuple::operator[](std::size_t i) const noexcept -> const value& { return at(i); }

auto tuple::operator[](std::size_t i) -> value&
{
  if (i >= values_.size())
    throw std::out_of_range{"cannot modify past the end of the tuple: you might want to use tuple::at instead"};
  return values_[i];
}

auto tuple::at(std::size_t i) const noexcept -> const value&
{
  return i < values_.size() ? values_[i] : values_.back();
}

function::function(std::function<tuple(tuple)> f) noexcept : f_(std::move(f)) {}

auto function::operator()(tuple t) const -> tuple { return f_(std::move(t)); }

} // namespace lua
