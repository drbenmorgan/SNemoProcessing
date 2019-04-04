#ifndef FALAISE_QUANTITY_H
#define FALAISE_QUANTITY_H

#include "bayeux/datatools/units.h"
#include "boost/metaparse/string.hpp"
#include "boost/mpl/string.hpp"
#include <exception>

namespace falaise {
  namespace units {
    //! Exception for wrong dimensions
    class wrong_dimension_error : public std::logic_error {
      using std::logic_error::logic_error;
    };

    //! Exception for unknown units
    class unknown_unit_error : public std::logic_error {
      using std::logic_error::logic_error;
    };

    //! Class representing a physical measurement
    /*
     *
     */
    class quantity {
    public:
      //! Default constructor
      quantity() = default;

      //! Construct a quantity
      /*
       * \param value Numeric value
       * \param unit Datatools::units tag
       * \throw falaise::unknown_unit_error if unit is not supported by
       * datatools::units
       */
      quantity(double value, std::string const& unit)
        : value_(value), unit_name(unit)
      {
        if (!datatools::units::find_unit(
              unit_name, unit_scale, dimension_name)) {
          throw unknown_unit_error{"unit '" + unit_name + "' is unknown"};
        }
      }

      virtual ~quantity() = default;

      //! Convert quantity to value in the CLHEP::Units system
      /*
       * \returns quantity's value in the CLHEP numeric units
       */
      operator double() const { return value_ * unit_scale; }

      //! Return the value for the quantity in its units
      /*
       * For example:
       *
       */
      double
      value() const
      {
        return value_;
      }

      //! Return value in CLHEP::Units unit
      /*
       * \throws bad_unit_error if supplied unit's dimension is different to
       * that of the quantity
       */
      double
      value_in(datatools::units::unit const& unit) const
      {
        if (unit.get_dimension_label() != dimension_name) {
          throw wrong_dimension_error("input unit dimension '" +
                                      unit.get_dimension_label() + "' != '" +
                                      dimension_name + "'");
        }

        return value_ * unit_scale / unit;
      }

      //! Return datatools::units tag of the quantity's unit
      std::string const&
      unit() const
      {
        return unit_name;
      }

      //! Return datatools::units tag of the quantity's dimension
      std::string const&
      dimension() const
      {
        return dimension_name;
      }

    private:
      double value_{0.0};
      std::string unit_name{""};
      std::string dimension_name{""};
      double unit_scale{1.0};
    };

    //! Type for "explicit" dimensions
    /*
     *
     */
    template <typename Dimension>
    class quantity_t : public quantity {
    public:
      quantity_t() = default;

      quantity_t(double value, std::string const& unit) : quantity(value, unit)
      {
        if (boost::mpl::c_str<typename Dimension::label>::value !=
            dimension()) {
          throw wrong_dimension_error(
            "dimension of unit '" + unit + "' is not '" +
            boost::mpl::c_str<typename Dimension::label>::value + "'");
        }
      }

      quantity_t(quantity const& q) : quantity_t(q.value(), q.unit()) {}

      virtual ~quantity_t() = default;
    };
  }
}

// Follow nholthaus units and macro-ize the boiler plate
#define FALAISE_ADD_DIMENSION_TAG(name)                                        \
  namespace falaise {                                                          \
    namespace units {                                                          \
      struct name {                                                            \
        typedef BOOST_METAPARSE_STRING(#name) label;                           \
      };                                                                       \
      using name##_t = quantity_t<name>;                                       \
    }                                                                          \
  }

// Explicit known dimensions
FALAISE_ADD_DIMENSION_TAG(absorbed_dose)
FALAISE_ADD_DIMENSION_TAG(acceleration)
FALAISE_ADD_DIMENSION_TAG(activity)
FALAISE_ADD_DIMENSION_TAG(amount)
FALAISE_ADD_DIMENSION_TAG(angle)
FALAISE_ADD_DIMENSION_TAG(angular_frequency)
FALAISE_ADD_DIMENSION_TAG(capacitance)
FALAISE_ADD_DIMENSION_TAG(conductance)
FALAISE_ADD_DIMENSION_TAG(conductivity)
FALAISE_ADD_DIMENSION_TAG(cross_section)
FALAISE_ADD_DIMENSION_TAG(data_storage)
FALAISE_ADD_DIMENSION_TAG(data_transfer_rate)
FALAISE_ADD_DIMENSION_TAG(density)
FALAISE_ADD_DIMENSION_TAG(electric_charge)
FALAISE_ADD_DIMENSION_TAG(electric_current)
FALAISE_ADD_DIMENSION_TAG(electric_displacement_field)
FALAISE_ADD_DIMENSION_TAG(electric_field)
FALAISE_ADD_DIMENSION_TAG(electric_flux)
FALAISE_ADD_DIMENSION_TAG(electric_potential)
FALAISE_ADD_DIMENSION_TAG(electric_resistance)
FALAISE_ADD_DIMENSION_TAG(electric_signal_integral)
FALAISE_ADD_DIMENSION_TAG(energy)
FALAISE_ADD_DIMENSION_TAG(equivalent_dose)
FALAISE_ADD_DIMENSION_TAG(force)
FALAISE_ADD_DIMENSION_TAG(fraction)
FALAISE_ADD_DIMENSION_TAG(frequency)
FALAISE_ADD_DIMENSION_TAG(illuminance)
FALAISE_ADD_DIMENSION_TAG(inductance)
FALAISE_ADD_DIMENSION_TAG(length)
FALAISE_ADD_DIMENSION_TAG(level)
FALAISE_ADD_DIMENSION_TAG(luminance)
FALAISE_ADD_DIMENSION_TAG(luminous_energy)
FALAISE_ADD_DIMENSION_TAG(luminous_energy_density)
FALAISE_ADD_DIMENSION_TAG(luminous_exposure)
FALAISE_ADD_DIMENSION_TAG(luminous_flux)
FALAISE_ADD_DIMENSION_TAG(luminous_intensity)
FALAISE_ADD_DIMENSION_TAG(magnetic_field_strength)
FALAISE_ADD_DIMENSION_TAG(magnetic_flux)
FALAISE_ADD_DIMENSION_TAG(magnetic_flux_density)
FALAISE_ADD_DIMENSION_TAG(mass)
FALAISE_ADD_DIMENSION_TAG(mass_activity)
FALAISE_ADD_DIMENSION_TAG(permeability)
FALAISE_ADD_DIMENSION_TAG(permittivity)
FALAISE_ADD_DIMENSION_TAG(power)
FALAISE_ADD_DIMENSION_TAG(pressure)
FALAISE_ADD_DIMENSION_TAG(procedure_defined)
FALAISE_ADD_DIMENSION_TAG(resistivity)
FALAISE_ADD_DIMENSION_TAG(solid_angle)
FALAISE_ADD_DIMENSION_TAG(surface)
FALAISE_ADD_DIMENSION_TAG(surface_activity)
FALAISE_ADD_DIMENSION_TAG(surface_density)
FALAISE_ADD_DIMENSION_TAG(surface_tension)
FALAISE_ADD_DIMENSION_TAG(temperature)
FALAISE_ADD_DIMENSION_TAG(time)
FALAISE_ADD_DIMENSION_TAG(velocity)
FALAISE_ADD_DIMENSION_TAG(volume)
FALAISE_ADD_DIMENSION_TAG(volume_activity)
FALAISE_ADD_DIMENSION_TAG(wave_number)

#endif /* FALAISE_QUANTITY_H */