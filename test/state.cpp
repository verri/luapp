#include <catch.hpp>

#include <luapp/state.hpp>
#include <luapp/value.hpp>

TEST_CASE("Basic state manipulation", "[state]")
{
  using namespace lua;

  const state s;
  const auto g = s.global_table();

  {
    const value foo = g.get("foo");
    CHECK(foo.is_nil());
  }

  s.do_string("foo = 10");
  {
    const value foo = g.get("foo");
    CHECK(foo.is_integer());
  }
}
