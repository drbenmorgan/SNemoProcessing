#include "catch.hpp"

#include "property_set.h"

#include "bayeux/datatools/units.h"
#include "bayeux/datatools/clhep_units.h"

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
                    falaise::wrong_type_error);
  REQUIRE_NOTHROW(ps.get<falaise::path>("apath"));
}

TEST_CASE("Insertion/Erase interfaces work", "")
{
  falaise::property_set ps{};
  ps.put("foo", 1);

  SECTION("putting the same key throws existing_key_error")
  {
    REQUIRE_THROWS_AS(ps.put("foo", 1),
                      falaise::existing_key_error);
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
                      falaise::wrong_type_error);
    REQUIRE_THROWS_AS(ps.get<int>("avector"),
                      falaise::wrong_type_error);
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
    REQUIRE_THROWS_AS(ps.get<std::string>("my_relpath"), falaise::wrong_type_error);

    REQUIRE(ps.get<falaise::path>("my_abspath") == abspth);
    REQUIRE_THROWS_AS(ps.get<std::string>("my_abspath"), falaise::wrong_type_error);
  }

  SECTION("env vars are expanded on get")
  {
    ps.put("home", falaise::path{"$HOME"});
    REQUIRE(ps.get<falaise::path>("home") == getenv("HOME"));
  }
}



TEST_CASE("Quantity type put/get specialization works", "")
{
  falaise::property_set ps;
  ps.put("number", 3.14);
  ps.put("quantity", falaise::quantity{4.13, "m"});
  // TODO: Cannot put quantity_t<U> type yet
  ps.put("amass", falaise::mass_t{4.13, "kg"});

  REQUIRE_THROWS_AS( ps.get<falaise::quantity>("number"), falaise::wrong_type_error );
  REQUIRE_THROWS_AS( ps.get<double>("quantity"), falaise::wrong_type_error );

  REQUIRE( ps.get<falaise::quantity>("quantity").value() == Approx(4.13) );
  REQUIRE( ps.get<falaise::quantity>("quantity").unit() == "m" );

  falaise::length_t q;
  REQUIRE_NOTHROW(q = ps.get<falaise::length_t>("quantity"));
  REQUIRE( q.value() == Approx(4.13) );
  REQUIRE( q.unit() == "m" );
  REQUIRE( q.dimension() == "length");
  REQUIRE( q == Approx(4.13*CLHEP::m) );

  //falaise::mass_t r {ps.get<falaise::quantity>("quantity")};
}



TEST_CASE("Property units/quantities", "")
{
  // So get rule for a quantity is:
  // - must be real
  // - must have explicit unit
  // - must have unit symbol
  // Thus put rule is
  // - Must set value, explicit unit flag, and unit symbol (with dimensional checks)
  //
  // NB: also means *should* not extract double unless it is dimensionless


  // What info do we get, if any, from a dimensioned real.
  datatools::properties ps;
  // Try and add stuff from a string
  // Bar is valid, it will be set with explicit unit and symbol (and properties parsing validates symbol matches dimension)
  // Foo is not valid, but accepted by properties, imported as a dimensionless scalar
  std::string myprops{
    "bar : real as velocity = 3.14 m/s \n"
    "foo : real as length = 2.0\n"
  };

  std::istringstream iput{myprops};
  datatools::properties::config reader;
  reader.read(iput, ps);

  ps.store_with_explicit_unit("weight", 1.2*CLHEP::kg);
  // Ugh, have to set unit symbol explicitly in code...
  ps.set_unit_symbol("weight", "g");


  ps.tree_dump(std::cout);
  std::cout << "weight: x" << ps.get_unit_symbol("weight") << "x\n";
  double val{0.0};

  // Gives the raw value in internal units (CLHEP) system
  ps.fetch("weight", val);
  // so must be converted
  std::cout << val/CLHEP::g << std::endl;

  // This is the same, but checks that value has explicit unit (though
  // as above, this just marks it as dimensioned)
  val = ps.fetch_real_with_explicit_unit("weight");
  std::cout << val << std::endl;

  // If we have a unit symbol, then we can get the "label"
  std::string unit_label{};
  double cFactor{0.0};
  bool hasLabel = datatools::units::find_unit(ps.get_unit_symbol("weight"), cFactor, unit_label);
  if (hasLabel) {
    std::cout << "weight has label" << unit_label << std::endl;
  }


  // All dimensions known
  std::vector<std::string> dims{};
  size_t N = datatools::units::registered_unit_dimension_labels(dims);
  for (auto& d : dims) {
    std::cout <<d << "\n";
  }

}
