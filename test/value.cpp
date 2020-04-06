#include <catch.hpp>

#include <luapp/value.hpp>

TEST_CASE("Basic value manipulation", "[value]")
{
  using namespace lua;

  value a;
  value b = 1ll;
  value c = 1.0;
  value d = "1";
  value e = false;
  value f = true;

  CHECK(!a);
  CHECK(b);
  CHECK(c);
  CHECK(d);
  CHECK(!e);
  CHECK(f);

  CHECK(a.is_nil());
  CHECK(b.is_integer());
  CHECK(c.is_floating());
  CHECK(d.is_string());
  CHECK(e.is_boolean());
  CHECK(f.is_boolean());
}

TEST_CASE("Functions", "[value]")
{
  using namespace lua;

  function f = +[](value) { return tuple{}; };
  function g(nargs<1>, [](value) { return tuple{}; });
}
