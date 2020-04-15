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

TEST_CASE("Functions called from lua", "[state]")
{
  using namespace lua;
  state s;
  table g = s.global_table();

  {
    function f = +[](value a, value b, value c) { return tuple{(bool)a, (bool)b, (bool)c}; };
    set(g, "f", f);

    {
      const auto [a, b, c] = s.do_string("return f()").expand(returns<3>);
      CHECK(!a);
      CHECK(!b);
      CHECK(!c);
    }

    {
      // missing arguments are nil
      const auto [a, b, c] = s.do_string("return f(1.0, 2.0)").expand(returns<3>);
      CHECK(a);
      CHECK(b);
      CHECK(!c);
    }

    {
      // extra returns are nil
      const auto [a, b, c, d] = s.do_string(returns<4>, "return f(1.0, nil, 2.0)");
      CHECK(a);
      CHECK(!b);
      CHECK(c);
      CHECK(!d);
      CHECK(d.is_nil());
    }

    {
      const auto [a, b, c] = s.do_string(returns<3>, "return f(false, 0, 0.0)");
      CHECK(!a);
      CHECK(b);
      CHECK(c);
    }
  }

  // extra arguments are ignored.
  {
    function f = +[]() {};
    set(g, "f", f);
    CHECK(s.do_string("return f(1.0, 2.0, 3.0)").size() == 0);
  }

  // if the number of arguments cannot be deduced, use args.
  {
    function f(args<2>, [](value, value) { return "hello"; });
    set(g, "f", f);
    value a = s.do_string("return f()");
    CHECK(a.is_string());
  }

  {
    struct custom_type
    {};

    function f = +[](std::shared_ptr<const custom_type> a, std::optional<floating> b,
                     std::variant<nil, integer, floating> c) {
      return tuple{(bool)a, (bool)b, std::holds_alternative<floating>(c)};
    };
    set(g, "f", f);
    set(g, "udata", userdata(s, std::in_place_type<custom_type>));

    {
      const auto [a, b, c] = s.do_string(returns<3>, "return f(udata, 1.0, 1)");
      CHECK(a);
      CHECK(b);
      CHECK(!c);
    }

    {
      const auto [a, b, c] = s.do_string(returns<3>, "return f(udata)");
      CHECK(a);
      CHECK(!b);
      CHECK(!c);
    }

    {
      const auto [a, b, c] = s.do_string(returns<3>, R"( return f(1.0, "wrong type", 1.0) )");
      CHECK(!a);
      CHECK(!b);
      CHECK(c);
    }

    {
      // integers are automatically converted to a valid std::optional<floating>
      const auto [a, b, c] = s.do_string(returns<3>, "return f(nil, 1, 2)");
      CHECK(!a);
      CHECK(b);
      CHECK(!c);
    }

    {
      const auto t = s.do_string("return f()");
      CHECK(!t[0]);
      CHECK(!t[1]);
      CHECK(!t[2]);
      CHECK(!t[3]);
      CHECK(!t[4]);
      // ...
    }
  }
}
