#include "catch.hpp"

#include "property_set.h"

// - Fixtures and helpers
datatools::properties
makeSampleProperties()
{
  datatools::properties test;
  test.store("foo", 1);
  test.store("bar", 3.14);
  test.store("baz", true);
  test.store("flatstring", "foobar");
  test.store_path("apath", "foobar");
  return test;
}

// Identified requirement: Need equivalent of ParameterSet's string/hash ops
// Equivalent to requirement of needing an equality operator!
// So also need an equivalence operator for comparing ps to props
//
// std::string
// to_string(falaise::property_set const& ps) {
//  datatools::properties tmp = ps;
//  std::ostringstream cv;
//  tmp.to_string(cv);
//  return cv.str();
//}

TEST_CASE("property_set default construction works", "")
{
  falaise::property_set ps;
  REQUIRE(ps.is_empty());
  REQUIRE(ps.get_names() == std::vector<std::string>{});
}

TEST_CASE("property_set construction from datatools work", "")
{
  falaise::property_set ps{makeSampleProperties()};
  REQUIRE(!ps.is_empty());

  auto names = ps.get_names();
  REQUIRE(names.size() == 5);

  for (auto& n : names) {
    REQUIRE(ps.has_key(n));
  }

  std::cout << ps.to_string() << std::endl;
}

TEST_CASE("Retriever interfaces work", "")
{
  falaise::property_set ps{makeSampleProperties()};

  REQUIRE(ps.get<int>("foo") == 1);
  REQUIRE_THROWS(ps.get<double>("foo"));

  REQUIRE(ps.get<int>("foo", 42) == 1);
  REQUIRE(ps.get<int>("off", 42) == 42);

  REQUIRE_THROWS_AS(ps.get<falaise::path>("flatstring"),
                    falaise::property_set::wrong_type_error);
  REQUIRE_NOTHROW(ps.get<falaise::path>("apath"));
}

TEST_CASE("Insertion/Erase interfaces work", "")
{
  falaise::property_set ps{};
  ps.put("foo", 1);

  SECTION("putting the same key throws existing_key_error")
  {
    REQUIRE_THROWS_AS(ps.put("foo", 1),
                      falaise::property_set::existing_key_error);
  }

  SECTION("replacing an existing key/value works")
  {
    REQUIRE_NOTHROW(ps.put_or_replace("foo", 2));
    REQUIRE(ps.get<int>("foo") == 2);

    REQUIRE_NOTHROW(ps.put_or_replace("foo", std::string{"foo"}));
    REQUIRE(ps.get<std::string>("foo") == "foo");
  }

  SECTION("putting/retriving explicit scalar/vector types works")
  {
    ps.put("ascalar", 1234);
    ps.put("avector", std::vector<int>{1, 2, 3, 4});

    REQUIRE_THROWS_AS(ps.get<std::vector<int>>("ascalar"),
                      falaise::property_set::wrong_type_error);
    REQUIRE_THROWS_AS(ps.get<int>("avector"),
                      falaise::property_set::wrong_type_error);
  }
}

TEST_CASE("Path type put/get specialization works", "")
{
  falaise::property_set ps{};
  
  SECTION("can only retrieve paths as paths")
  {
    falaise::path relpth{"relpath"};
    falaise::path abspth{"/tmp"};

    ps.put("my_relpath", relpth);
    ps.put("my_abspath", abspth);

    REQUIRE(ps.get<falaise::path>("my_relpath") == relpth);
    REQUIRE_THROWS_AS(ps.get<std::string>("my_relpath"), falaise::property_set::wrong_type_error);

    REQUIRE(ps.get<falaise::path>("my_abspath") == abspth);
    REQUIRE_THROWS_AS(ps.get<std::string>("my_abspath"), falaise::property_set::wrong_type_error);
  }

  SECTION("env vars are expanded on get")
  {
    ps.put("home", falaise::path{"$HOME"});
    REQUIRE(ps.get<falaise::path>("home") == getenv("HOME"));
  }
}
