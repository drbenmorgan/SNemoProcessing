#ifndef FALAISE_PROPERTY_SET_H
#define FALAISE_PROPERTY_SET_H

#include "bayeux/datatools/properties.h"
#include "bayeux/datatools/units.h"
#include "boost/metaparse/string.hpp"
#include "boost/mpl/contains.hpp"
#include "boost/mpl/string.hpp"
#include "boost/mpl/vector.hpp"


namespace falaise {
  // Minimal type for paths enabling template specialization
  // Need to check what comes out of properties parsing to see if further
  // expansion is needed...
  // Can probably do the same for quantities
  class path {
  public:
    path() = default;
    path(std::string const& p) : value(p) {}

    operator std::string() const { return value; }

    bool
    operator==(path const& other) const
    {
      return value == other.value;
    }

    bool
    operator==(std::string const& other) const
    {
      return value == other;
    }

  private:
    std::string value;
  };

  std::ostream&
  operator<<(std::ostream& os, path const& p)
  {
    os << std::string{p};
    return os;
  }




  // Basic type for a quantity
  class quantity {
  public:
    using bad_unit_error = std::logic_error;
    using bad_dimension_error = std::logic_error;

    quantity() = default;
    quantity(double value, std::string const& unit) : value_(value), unit_name(unit)
    {
      if (!datatools::units::find_unit(unit_name, unit_scale, dimension_name)) {
        throw bad_unit_error("no dimensional information for unit '" + unit_name +
                             "'");
      }
    }

    virtual ~quantity()=default;

    //! Convert quantity to value in the CLHEP::Units system
    operator double() const { return value_ * unit_scale; }

    //! Return the concrete value for the quantity in its units
    double
    value() const
    {
      return value_;
    }

    //! Return value for the quantity in the given units
    //! \throws bad_unit_error if dimension if supplied unit is different to that of the quantity
    double
    value_in(datatools::units::unit const& unit) const
    {
      if (unit.get_dimension_label() != dimension_name) {
        throw bad_dimension_error("different dimensions!!");
      }

      return value_ * unit_scale / unit;
    }

    //! Return string representation of the quantity's units
    std::string const&
    unit() const
    {
      return unit_name;
    }

    //! Return string representation of the quantity's dimensions
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


  // Type for "explicit" dimensions
  template <typename Dimension>
  class quantity_t : public quantity {
   public:
    quantity_t()=default;

    quantity_t(double value, std::string const& unit) : quantity(value, unit) {
      if(boost::mpl::c_str<typename Dimension::label>::value != dimension()) {
        throw bad_dimension_error("Dimension of unit '"+unit+"' is not '"+boost::mpl::c_str<typename Dimension::label>::value+"'");
      }
    }

    quantity_t(quantity const& q) : quantity_t(q.value(), q.unit()) {}

    virtual ~quantity_t()=default;
  };


  // Follow nholthaus units and macro-ize the boiler plate
  #define FALAISE_ADD_DIMENSION_TAG(name) \
  struct name\
  {\
    typedef BOOST_METAPARSE_STRING(#name) label; \
  };\
  using name ## _t = ::falaise::quantity_t<name>;

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
  // Mass

  // Time

  // electric_current

  // temperature

  // amount

  // surface (Area)

  // surface_density (Mass/Area)

  // cross_section (synonym for Area, but requires units to be given in barns)

  // volume

  // angle

  // solid_angle

  // energy

  // force

  // pressure

  // density

  // frequency

  // activity (e.g. Bequerel)

  // volume_activity

  // surface_activity

  // mass_activity

  // fraction

  // velocity

  // acceleration

  // electric_charge

  // electric_potential

  // electric_resistance

  // resistivity

  // power

  // capacitance

  // electric_field

  //



  class property_set {
  public:
    using missing_key_error = std::logic_error;
    using existing_key_error = std::logic_error;
    using wrong_type_error = std::logic_error;

    // Need default c'tor as we have user-defined c'tor
    property_set() = default;
    property_set(datatools::properties const& ps);

    // - Observers
    //! Returns true if no key/values pairs are held
    bool is_empty() const;

    //! Returns a list of all keys in the property_set
    std::vector<std::string> get_names() const;

    //! Returns true if the property_set contains a pair with the supplied key
    bool has_key(std::string const& key) const;

    //! Returns a string representation of the property_set
    std::string to_string() const;

    //! Convert back to datatools::properties
    operator datatools::properties() const;

    // - Retrievers

    //! Return the value of type T associated with supplied key
    template <typename T>
    T get(std::string const& key) const;

    //! Return the value of type T associated with key, or default if the key is
    // not present
    template <typename T>
    T get(std::string const& key, T const& default_value) const;

    // - Inserters

    //! Insert key-value pair in property_set, throwing if key already exist
    template <typename T>
    void put(std::string const& key, T const& value);

    //! Insert with replace
    template <typename T>
    void put_or_replace(std::string const& key, T const& value);

    // - Deleters:

    //! Erase the name-value pair matching name, returning true on success,
    // false otherwise
    bool erase(std::string const& key);

  private:
    //! List of types that property_set can hold
    using types_ = boost::mpl::vector<int,
                                      double,
                                      bool,
                                      std::string,
                                      path,
                                      quantity,
                                      std::vector<int>,
                                      std::vector<double>,
                                      std::vector<bool>,
                                      std::vector<std::string>>;
    //! Compile-time check that T can be held
    /*
     * can_hold_<T>::value : true if T can be held, false otherwise
     */
    template <typename T>
    struct can_hold_ {
      typedef typename boost::mpl::contains<types_, T>::type type;
    };

    template <typename T>
    struct can_hold_<quantity_t<T>> {
      typedef std::true_type type;
    };

    //! Return true if value held at key has type T
    /*
     * Assert that T be a holdable type before dispatching to the
     * implementation function for the specific type
     */
    template <typename T>
    bool is_type_(std::string const& key) const;

    //! Return true if value at key has type int
    bool is_type_impl_(std::string const& key, int) const;

    //! Return true if value at key has type double
    bool is_type_impl_(std::string const& key, double) const;

    //! Return true if value at key has type bool
    bool is_type_impl_(std::string const& key, bool) const;

    //! Return true if value at key has type std::string
    bool is_type_impl_(std::string const& key, std::string) const;

    //! Return true if value at key has type falaise::path
    bool is_type_impl_(std::string const& key, path) const;

    //! Return true if value at key has type falaise::quantity
    bool is_type_impl_(std::string const& key, quantity) const;

    //! Return true if value at key has type std::vector<int>
    bool is_type_impl_(std::string const& key, std::vector<int>) const;

    //! Return true if value at key has type std::vector<double>
    bool is_type_impl_(std::string const& key, std::vector<double>) const;

    //! Return true if value at key has type std::vector<bool>
    bool is_type_impl_(std::string const& key, std::vector<bool>) const;

    //! Return true if value at key has type std::vector<std::string>
    bool is_type_impl_(std::string const& key, std::vector<std::string>) const;

    //! Set result to value held at key
    /*
     * Specialize/overload this for any T requiring type conversion (e.g. path, quantity)
     */
    template <typename T>
    void fetch_impl_(std::string const& key, T& result) const;

    //! Overloaded fetch for explicitly dimensioned quantities
    template <typename T>
    void fetch_impl_(std::string const& key, quantity_t<T>& result) const;

    datatools::properties ps_; //! Wrapped instance of datatools::properties
  };
} /* falaise */

namespace falaise {
  property_set::property_set(datatools::properties const& ps) : ps_(ps) {}

  property_set::operator datatools::properties() const { return ps_; }

  bool
  property_set::is_empty() const
  {
    return ps_.empty();
  }

  std::vector<std::string>
  property_set::get_names() const
  {
    return ps_.keys();
  }

  bool
  property_set::has_key(std::string const& key) const
  {
    return ps_.has_key(key);
  }

  std::string
  property_set::to_string() const
  {
    std::ostringstream oss;
    ps_.tree_dump(oss);
    return oss.str();
  }

  template <typename T>
  T
  property_set::get(std::string const& key) const
  {
    // Check key ourselves so we can throw a more informative error
    if (!ps_.has_key(key)) {
      throw missing_key_error("property_set does not hold a key '" + key + "'");
    }
    if (!is_type_<T>(key)) {
      throw wrong_type_error("value at '" + key + "' is not of requested type");
    }

    T result;
    fetch_impl_(key, result);
    return result;
  }

  template <typename T>
  T
  property_set::get(std::string const& key, T const& default_value) const
  {
    T result{default_value};
    if (ps_.has_key(key)) {
      if (!is_type_<T>(key)) {
        throw wrong_type_error("value at '" + key +
                               "' is not of requested type");
      }
      fetch_impl_(key, result);
    }
    return result;
  }

  template <typename T>
  void
  property_set::put(std::string const& key, T const& value)
  {
    static_assert(can_hold_<T>::type::value,
                  "property_set cannot hold values of type T");
    // Check directly to use our clearer exception type
    if (ps_.has_key(key)) {
      throw existing_key_error{"property_set already contains key " + key};
    }
    ps_.store(key, value);
  }

  // Specialization for path type
  template <>
  void
  property_set::put(std::string const& key, path const& value)
  {
    // Check directly to use our clearer exception type
    if (ps_.has_key(key)) {
      throw existing_key_error{"property_set already contains key " + key};
    }

    ps_.store_path(key, value);
  }

  // Specialization for quantity types, including quantity_t<T>s
  template <>
  void
  property_set::put(std::string const& key, quantity const& value)
  {
    // Check directly to use our clearer exception type
    if (ps_.has_key(key)) {
      throw existing_key_error{"property_set already contains key " + key};
    }

    // Need to think about how values are transformed...
    ps_.store_with_explicit_unit(key, value.value());
    ps_.set_unit_symbol(key, value.unit());
  }

  template <typename T>
  void
  property_set::put_or_replace(std::string const& key, T const& value)
  {
    // Cannot change type of already held data, so must erase/re-store
    erase(key);
    put(key, value);
  }

  bool
  property_set::erase(std::string const& key)
  {
    // Check first to avoid exception from erase() of properties
    if (ps_.has_key(key)) {
      ps_.erase(key);
      return true;
    }
    return false;
  }

  template <typename T>
  bool
  property_set::is_type_(std::string const& key) const
  {
    static_assert(can_hold_<T>::type::value,
                  "property_set cannot hold values of type T");
    if (ps_.has_key(key)) {
      return is_type_impl_(key, T{});
    }
    // Absence of key is false (clearly cannot be T)
    return false;
  }

  bool
  property_set::is_type_impl_(std::string const& key, int) const
  {
    return ps_.is_integer(key) && ps_.is_scalar(key);
  }

  bool
  property_set::is_type_impl_(std::string const& key, double) const
  {
    // Assume extraction of double is always dimensionless
    return ps_.is_real(key) && (!ps_.has_explicit_unit(key)) &&
           (!ps_.has_unit_symbol(key)) && ps_.is_scalar(key);
  }

  bool
  property_set::is_type_impl_(std::string const& key, bool) const
  {
    return ps_.is_boolean(key) && ps_.is_scalar(key);
  }

  bool
  property_set::is_type_impl_(std::string const& key, std::string) const
  {
    return ps_.is_string(key) && (!ps_.is_explicit_path(key)) &&
           ps_.is_scalar(key);
  }

  bool
  property_set::is_type_impl_(std::string const& key, path) const
  {
    return ps_.is_explicit_path(key) && ps_.is_scalar(key);
  }

  bool
  property_set::is_type_impl_(std::string const& key, quantity) const
  {
    // Quantity must be real, and have both explicit unit and unit symbol
    return ps_.is_real(key) && ps_.has_explicit_unit(key) &&
           ps_.has_unit_symbol(key) && ps_.is_scalar(key);
  }

  bool
  property_set::is_type_impl_(std::string const& key, std::vector<int>) const
  {
    return ps_.is_integer(key) && ps_.is_vector(key);
  }

  bool
  property_set::is_type_impl_(std::string const& key, std::vector<double>) const
  {
    // vector of doubles is always dimensionless
    return ps_.is_real(key) && (!ps_.has_explicit_unit(key)) &&
           (!ps_.has_unit_symbol(key)) && ps_.is_vector(key);
  }

  bool
  property_set::is_type_impl_(std::string const& key, std::vector<bool>) const
  {
    return ps_.is_boolean(key) && ps_.is_vector(key);
  }

  bool
  property_set::is_type_impl_(std::string const& key,
                              std::vector<std::string>) const
  {
    return ps_.is_string(key) && ps_.is_vector(key);
  }

  template <typename T>
  void
  property_set::fetch_impl_(std::string const& key, T& result) const
  {
    ps_.fetch(key, result);
  }

  // Full specialization for path type
  template <>
  void
  property_set::fetch_impl_(std::string const& key, falaise::path& result) const
  {
    result = falaise::path{ps_.fetch_path(key)};
  }

  // Full specialization for quantity type
  template <>
  void
  property_set::fetch_impl_(std::string const& key,
                            falaise::quantity& result) const
  {
    result = {ps_.fetch_real_with_explicit_unit(key), ps_.get_unit_symbol(key)};
  }

  // Overload for explicitly dimensioned quantities
  template <typename T>
  void
  property_set::fetch_impl_(std::string const& key,
                            falaise::quantity_t<T>& result) const
  {
    result = {ps_.fetch_real_with_explicit_unit(key), ps_.get_unit_symbol(key)};
  }

} /* falaise */

#endif // FALAISE_PROPERTY_SET_H
