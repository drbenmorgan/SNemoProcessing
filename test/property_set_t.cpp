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

TEST_CASE("Inserter interfaces work", "")
{
  falaise::property_set ps{makeSampleProperties()};

  REQUIRE(ps.erase("foo"));
  REQUIRE_FALSE(ps.has_key("foo"));
  REQUIRE_NOTHROW(ps.erase("off"));
  REQUIRE_FALSE(ps.erase("foo"));

  REQUIRE_THROWS_AS(ps.put("bar", 3.14),
                    falaise::property_set::existing_key_error);
  REQUIRE_NOTHROW(ps.put_or_replace("bar", 4.13));
  REQUIRE(ps.get<double>("bar") == Approx(4.13));
}
