#ifndef FALAISE_PATH_H
#define FALAISE_PATH_H

#include <string>
#include <ostream>

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
}

#endif /* FALAISE_PATH_H */