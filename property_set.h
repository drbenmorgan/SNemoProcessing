#ifndef FALAISE_PROPERTY_SET_H
#define FALAISE_PROPERTY_SET_H

#include "bayeux/datatools/properties.h"
#include "boost/mpl/contains.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/utility/enable_if.hpp"

namespace falaise {
  // Minimal type for paths enabling template specialization
  // Need to check what comes out of properties parsing to see if further
  // expansion is needed...
  // Can probably do the same for quantities
  class path {
   public:
    path()=default;
    path(std::string const& p) : value(p){}

    operator std::string() const { return value; }

   private:
    std::string value;
  };

  class property_set {
  public:
    using missing_key_error = std::logic_error;
    using existing_key_error = std::logic_error;
    using wrong_type_error = std::logic_error;

    // property_set can only hold a limited set of types
    // Need to think about types for Paths/Quantities...
    using types = boost::mpl::vector<int,
                                     double,
                                     bool,
                                     std::string,
                                     std::vector<int>,
                                     std::vector<double>,
                                     std::vector<bool>,
                                     std::vector<std::string>,
                                     path>;

    // Need default c'tor as we have user-defined c'tor
    property_set() = default;
    property_set(datatools::properties const& ps);

    operator datatools::properties() const;

    // - Observers

    //! Returns true if no key/values pairs are held
    bool is_empty() const;

    //! Returns a list of all keys in the property_set
    std::vector<std::string> get_names() const;

    //! Returns true if the property_set contains a pair with the supplied key
    bool has_key(std::string const& key) const;

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

    //! Erase the name-value pair matching name, returning true on success,
    // false otherwise
    bool erase(std::string const& key);

  private:
    datatools::properties ps_;
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

  template <typename T>
  T
  property_set::get(std::string const& key) const
  {
    // Simplest possible interface, but needs further checking
    // Should return value if present, throw otherwise (and needs type
    // checking!)
    static_assert(boost::mpl::contains<types, T>::type::value,
                  "property_set cannot hold values of type T");
    // Check key ourselves so we can throw a more informative error
    if (! ps_.has_key(key)) {
      throw missing_key_error("property_set does not hold a key '"+key+"'");
    }
    T result;
    ps_.fetch(key, result);
    return result;
  }

  template <typename T>
  T
  property_set::get(std::string const& key, T const& default_value) const
  {
    static_assert(boost::mpl::contains<types, T>::type::value,
                  "property_set cannot hold values of type T");
    T result{default_value};
    if (ps_.has_key(key)) {
      ps_.fetch(key, result);
    }
    return result;
  }

  template <typename T>
  void
  property_set::put(std::string const& key, T const& value)
  {
    static_assert(boost::mpl::contains<types, T>::type::value,
                  "property_set cannot hold values of type T");
    // Check directly to use our clearer exception type
    if (ps_.has_key(key)) {
      throw existing_key_error{"property_set already contains key " + key};
    }
    ps_.store(key, value);
  }

  template <typename T>
  void
  property_set::put_or_replace(std::string const& key, T const& value)
  {
    static_assert(boost::mpl::contains<types, T>::type::value,
                  "property_set cannot hold values of type T");
    ps_.update(key, value);
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

  // Full specialization for getting path type values
  template <>
  path property_set::get(std::string const& key) const
  {
    // Check key ourselves so we can throw a more informative error
    if (! ps_.has_key(key)) {
      throw missing_key_error("property_set does not hold a key '"+key+"'");
    }

    using data_t = datatools::properties::data;
    data_t x = ps_.get(key);
    if (!x.is_path()) {
      throw wrong_type_error("value at key "+key+" is not of type falaise::path");
    }
    return path{x.get_string_value()};
  }

} /* falaise */

#endif // FALAISE_PROPERTY_SET_H
