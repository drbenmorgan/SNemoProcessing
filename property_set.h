#ifndef FALAISE_PROPERTY_SET_H
#define FALAISE_PROPERTY_SET_H

#include "bayeux/datatools/properties.h"
#include "bayeux/datatools/units.h"
#include "boost/metaparse/string.hpp"
#include "boost/mpl/contains.hpp"
#include "boost/mpl/string.hpp"
#include "boost/mpl/vector.hpp"

namespace falaise {
  //! Class representing a property_set filesystem path
  /*
   * Filesystem paths can be defined in datatools::properties syntax as:
   *
   *   mypath : string as path = "/some/path"
   *
   * Parsing of the properties file will expand any mount points or environment
   * variables so that fetching the path will return a std::string containing
   * the absolute path.
   *
   * @ref path provides a simple type to distinguish raw std::string from
   * explicit paths. It wraps the std::string for the absolute path. @ref
   * falaise::property_set supports get/put of `path`s, validating the needed
   * attributes for retrieval or storage.
   */
  class path {
  public:
    //! Default constructor
    path() = default;

    //! Construct path from a std::string
    /*
     * Not declared explicit as we allow conversions for compatibility with
     * existing use of filesystem paths as std::string
     */
    path(std::string const& p) : value(p) {}

    //! Conversion to std::string
    operator std::string() const { return value; }

    //! Compare path with another
    /*
     * \returns true if the two operands are the same
     */
    bool
    operator==(path const& other) const
    {
      return value == other.value;
    }

    bool
    operator!=(path const& other) const
    {
      return !(*this == other);
    }

    //! Compare path with a string
    bool
    operator==(std::string const& other) const
    {
      return value == other;
    }

    bool
    operator!=(std::string const& other) const
    {
      return !(*this == other);
    }

  private:
    std::string value; //< absolute path value
  };

  //! Output path to an ostream
  std::ostream&
  operator<<(std::ostream& os, path const& p)
  {
    os << std::string{p};
    return os;
  }

  // -- QUANTITY
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

// Follow nholthaus units and macro-ize the boiler plate
#define FALAISE_ADD_DIMENSION_TAG(name)                                        \
  struct name {                                                                \
    typedef BOOST_METAPARSE_STRING(#name) label;                               \
  };                                                                           \
  using name##_t = ::falaise::units::quantity_t<name>;

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
  } /* units */

  // -- PROPERTY_SET

  //! Exception thrown when requesting a key that is not in the property_set
  class missing_key_error : public std::logic_error {
    using std::logic_error::logic_error;
  };

  //! Exception thrown when trying to put to a key already in the property_set
  class existing_key_error : public std::logic_error {
    using std::logic_error::logic_error;
  };

  //! Exception thrown when type requested in get<T>(key) does not match that of
  //! value at key
  class wrong_type_error : public std::logic_error {
    using std::logic_error::logic_error;
  };

  //! Class holding a set of key-value properties
  /*
   *  Provides a convenient adaptor interface over datatools::properties,
   * targeted at developers of modules for Falaise
   */
  class property_set {
  public:
    //! Default constructor
    property_set() = default;

    //! Construct from an existing datatools::properties
    /*
     *  \param[in] ps properties
     */
    property_set(datatools::properties const& ps);

    // - Observers
    //! Returns true if no key-value pairs are held
    bool is_empty() const;

    //! Returns a vector of all keys in the property_set
    std::vector<std::string> get_names() const;

    //! Returns true if the property_set contains a pair with the supplied key
    bool has_key(std::string const& key) const;

    //! Returns a string representation of the property_set
    std::string to_string() const;

    // - Retrievers
    //! Return the value of type T associated with supplied key
    /*
     * \tparam T type to be returned
     * \param[in] key key of value to be returned
     * \throw missing_key_error if key is not held
     * \throw wrong_type_error if value at key is not T
     */
    template <typename T>
    T get(std::string const& key) const;

    //! Return the value of type T associated with key, or default if the key is
    // not present
    template <typename T>
    T get(std::string const& key, T const& default_value) const;

    //! Convert back to datatools::properties
    operator datatools::properties() const;

    // - Inserters
    //! Insert key-value pair in property_set, throwing if key is already held
    template <typename T>
    void put(std::string const& key, T const& value);

    //! Insert key-value pair in property_set, replacing value if key exists
    template <typename T>
    void put_or_replace(std::string const& key, T const& value);

    // - Deleters:
    //! Erase the name-value pair matching name, returning true on success,
    // false otherwise
    bool erase(std::string const& key);

  private:
    //! \typedef List of types that property_set can hold
    using types_ = boost::mpl::vector<int,
                                      double,
                                      bool,
                                      std::string,
                                      path,
                                      units::quantity,
                                      std::vector<int>,
                                      std::vector<double>,
                                      std::vector<bool>,
                                      std::vector<std::string>>;
    template <typename T>
    struct can_hold_ {
      typedef typename boost::mpl::contains<types_, T>::type type;
    };

    template <typename T>
    struct can_hold_<units::quantity_t<T>> {
      typedef std::true_type type;
    };

    //! Compile-time check that T can be held
    /*
     * can_hold_t_<T>::value : true if T can be held, false otherwise
     */
    template <typename T>
    using can_hold_t_ = typename can_hold_<T>::type;

    //! Return true if value held at key has type T
    /*
     * Assert that T be a holdable type before dispatching to the
     * implementation function checking the specific type
     */
    template <typename T>
    bool is_type_(std::string const& key) const;

    //! Return true if value at key is an int
    bool is_type_impl_(std::string const& key, int) const;

    //! Return true if value at key is a dimensionless double
    bool is_type_impl_(std::string const& key, double) const;

    //! Return true if value at key is a bool
    bool is_type_impl_(std::string const& key, bool) const;

    //! Return true if value at key is a non-path std::string
    bool is_type_impl_(std::string const& key, std::string) const;

    //! Return true if value at key is a falaise::path
    bool is_type_impl_(std::string const& key, path) const;

    //! Return true if value at key is a falaise::quantity
    bool is_type_impl_(std::string const& key, units::quantity) const;

    //! Return true if value at key is a std::vector<int>
    bool is_type_impl_(std::string const& key, std::vector<int>) const;

    //! Return true if value at key is a std::vector<double> (dimensionless
    //! doubles)
    bool is_type_impl_(std::string const& key, std::vector<double>) const;

    //! Return true if value at key is a std::vector<bool>
    bool is_type_impl_(std::string const& key, std::vector<bool>) const;

    //! Return true if value at key is a std::vector<std::string>
    bool is_type_impl_(std::string const& key, std::vector<std::string>) const;

    //! Set result to value held at key
    /*
     * Dispatches to specializations or overloads as known for T
     */
    template <typename T>
    void fetch_impl_(std::string const& key, T& result) const;

    //! Overloaded fetch_impl_ for explicitly dimensioned quantities
    template <typename T>
    void fetch_impl_(std::string const& key, units::quantity_t<T>& result) const;

    datatools::properties ps_; //< underlying set of properties
  };
} /* falaise */

namespace falaise {
  
  template <typename T>
  T
  property_set::get(std::string const& key) const
  {
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
    static_assert(can_hold_t_<T>::value,
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
  property_set::put(std::string const& key, units::quantity const& value)
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

  template <typename T>
  bool
  property_set::is_type_(std::string const& key) const
  {
    static_assert(can_hold_t_<T>::value,
                  "property_set cannot hold values of type T");
    if (ps_.has_key(key)) {
      return is_type_impl_(key, T{});
    }
    // Absence of key is false (clearly cannot be T)
    return false;
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
  property_set::fetch_impl_(std::string const& key, path& result) const
  {
    result = falaise::path{ps_.fetch_path(key)};
  }

  // Full specialization for quantity type
  template <>
  void
  property_set::fetch_impl_(std::string const& key,
                            units::quantity& result) const
  {
    result = {ps_.fetch_real_with_explicit_unit(key), ps_.get_unit_symbol(key)};
  }

  // Overload for explicitly dimensioned quantities
  template <typename T>
  void
  property_set::fetch_impl_(std::string const& key,
                            units::quantity_t<T>& result) const
  {
    result = {ps_.fetch_real_with_explicit_unit(key), ps_.get_unit_symbol(key)};
  }

  //! Construct a property_set from an input datatools::properties file
  /*
   * \param filename File from which to read data
   * \param ps property_set to fill with data
   */
  void
  make_property_set(const std::string& filename, property_set& ps);
} /* falaise */

#endif // FALAISE_PROPERTY_SET_H
