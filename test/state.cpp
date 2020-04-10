#include <catch.hpp>

#include <luapp/state.hpp>
#include <luapp/value.hpp>

TEST_CASE("Basic state manipulation", "[state]")
{
  using namespace lua;

  const state s;
  const auto g = s.global_table();

  {
    const value foo = get(g, "foo");
    CHECK(foo.is_nil());
  }

  s.do_string("foo = 10");
  {
    const value foo = get(g, "foo");
    CHECK(foo.is_integer());
  }

  {
    const auto result = s.do_string(R"(return 1, 1.0, "1")");
    REQUIRE(result.size() == 3);

    const auto [a, b, c] = result.expand(args<3>);
    CHECK(a.is_integer());
    CHECK(b.is_floating());
    CHECK(c.is_string());
  }

  s.do_string("t = {}");
  const std::optional<table> t = get(g, "t");

  REQUIRE(t);
  set(*t, "bar", "bar");

  {
    const value bar = s.do_string("return t.bar");
    CHECK(bar.is_string());
    CHECK(bar.get_string_or("") == "bar");
  }

  {
    const value bar = get(*t, "bar");
    CHECK(bar.is_string());
    CHECK(bar.get_string_or("") == "bar");
  }
}
