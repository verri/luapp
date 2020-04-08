#include <catch.hpp>

#include <luapp/value.hpp>
#include <utility>
#include <variant>

TEST_CASE("Basic value manipulation", "[value]")
{
  using namespace lua;

  const value a;
  const value b = integer{1};
  const value c = floating{1.0};
  const value d = "1";
  const value e = false;
  const value f = true;

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

  const auto x = static_cast<std::optional<string>>(a);
  CHECK(!x.has_value());

  std::variant<nil, integer, floating> y;

  y = b;
  CHECK(std::holds_alternative<integer>(y));

  y = c;
  CHECK(std::holds_alternative<floating>(y));

  struct custom_type
  {};

  const value g = userdata(std::in_place_type<custom_type>);
  CHECK(g.is_userdata());
  CHECK(static_cast<std::shared_ptr<custom_type>>(g));
}

TEST_CASE("Functions", "[value]")
{
  using namespace lua;

  {
    function f = +[](value a, value b, value c) { return tuple{(bool)a, (bool)b, (bool)c}; };

    {
      const auto [a, b, c] = f().expand(nrets<3>);
      CHECK(!a);
      CHECK(!b);
      CHECK(!c);
    }

    {
      // missing arguments are nil
      const auto [a, b, c] = f(1.0, 2.0).expand(nrets<3>);
      CHECK(a);
      CHECK(b);
      CHECK(!c);
    }

    {
      // extra returns are nil
      const auto [a, b, c, d] = f(nrets<4>, 1.0, nil{}, 2.0);
      CHECK(a);
      CHECK(!b);
      CHECK(c);
      CHECK(!d);
      CHECK(d.is_nil());
    }

    {
      const auto [a, b, c] = f(nrets<3>, false, integer{0}, floating{0});
      CHECK(!a);
      CHECK(b);
      CHECK(c);
    }
  }

  // extra arguments are ignored.
  {
    function f = +[]() {};
    CHECK(f(1.0, 2.0, 3.0).size() == 0);
  }

  // if the number of arguments cannot be deduced, use nargs.
  {
    function f(nargs<2>, [](value, value) { return "hello"; });
    value a = f();
    CHECK(a.is_string());
  }

  {
    struct custom_type
    {};

    function f = +[](std::shared_ptr<custom_type> a, std::optional<floating> b,
                     std::variant<nil, integer, floating> c) {
      return tuple{(bool)a, (bool)b, std::holds_alternative<floating>(c)};
    };

    {
      const auto udata = userdata(std::in_place_type<custom_type>);
      const auto [a, b, c] = f(nrets<3>, udata, 1.0, integer{1});
      CHECK(a);
      CHECK(b);
      CHECK(!c);
    }

    {
      const auto udata = std::make_shared<custom_type>();
      const auto [a, b, c] = f(nrets<3>, udata);
      CHECK(a);
      CHECK(!b);
      CHECK(!c);
    }

    {
      const auto [a, b, c] = f(nrets<3>, 1.0, "wrong type", 1.0);
      CHECK(!a);
      CHECK(!b);
      CHECK(c);
    }

    {
      const auto t = f();
      CHECK(!t[0]);
      CHECK(!t[1]);
      CHECK(!t[2]);
      CHECK(!t[3]);
      CHECK(!t[4]);
      // ...
    }
  }
}
