#include "catch.hpp"

#include "boost/units/io.hpp"
#include "boost/units/systems/si.hpp"
#include "boost/units/systems/si/prefixes.hpp"
#include "boost/units/systems/cgs.hpp"
#include "boost/units/quantity.hpp"

// HEP/CLHEP units are basically rescaled SI units?
namespace boost {
namespace units {
namespace clhep {

}
}
} // namespace boost

#include <iostream>

TEST_CASE("sanity", "")
{
  using namespace boost::units;
  quantity<si::length> mSI(2.4*si::centi*si::metre);
  quantity<cgs::length> mCGS(mSI);
  std::cout << mSI << ", " << mCGS << std::endl;
  // This gives use the value in the base unit
  std::cout << mSI.value() << ", " << mCGS.value() << std::endl;

  // Can only implicitly convert with an explicit conversion to dimensionless
  // These give hetrogenous units, so not so clean!
  std::cout << mCGS / cgs::centimeter << ", " << mCGS / si::meter << std::endl;

  // Better to use value(), or explicit conversion (as above for mCGS(mSI)?)
}
