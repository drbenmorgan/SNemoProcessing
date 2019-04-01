#ifndef FALAISE_PROPERTY_SET_H
#define FALAISE_PROPERTY_SET_H

#include "bayeux/datatools/properties.h"
#include "boost/mpl/contains.hpp"
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
                                      std::vector<int>,
                                      std::vector<double>,
                                      std::vector<bool>,
                                      std::vector<std::string>>;
    //! Compile-time check that T can be held
    /*
     * can_hold_<T>::value : true if T can be held, false otherwise
     */
    template <typename T>
    using can_hold_ = typename boost::mpl::contains<types_, T>::type;

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
     * Specialize this for any T requiring type conversion (e.g. path, quantity)
     */
    template <typename T>
    void fetch_impl_(std::string const& key, T& result) const;

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
    static_assert(can_hold_<T>::value,
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

  template <typename T>
  void
  property_set::put_or_replace(std::string const& key, T const& value)
  {
    // Cannot update type of held data, so must/erase/re-store
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
    static_assert(can_hold_<T>::value,
                  "property_set cannot hold values of type T");
    if (ps_.has_key(key)) {
      return is_type_impl_(key, T{});
    }
    // Absence of key is false
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
    return ps_.is_real(key) && ps_.is_scalar(key);
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
  property_set::is_type_impl_(std::string const& key, std::vector<int>) const
  {
    return ps_.is_integer(key) && ps_.is_vector(key);
  }

  bool
  property_set::is_type_impl_(std::string const& key, std::vector<double>) const
  {
    return ps_.is_real(key) && ps_.is_vector(key);
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

} /* falaise */

#endif // FALAISE_PROPERTY_SET_H
