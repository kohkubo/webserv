#ifndef SRCS_CONFIG_LOCATION_HPP
#define SRCS_CONFIG_LOCATION_HPP

#include <map>
#include <string>
#include <vector>

#include "utils/Path.hpp"
#include "utils/tokenize.hpp"

namespace config {

typedef std::map<int, std::string> returnMap;

class Location {
public:
  struct MethodFlag {
    bool get_;
    bool post_;
    bool delete_;
  };

  Path       location_path_;
  Path       root_;
  Path       index_;
  bool       autoindex_;
  bool       cgi_extension_;
  bool       upload_file_;
  returnMap  return_map_;
  bool       has_available_methods_;
  MethodFlag available_methods_;

private:
  tokenIterator _parse_available_methods(tokenIterator pos, tokenIterator end);

public:
  Location()
      : root_("html/")
      , index_("index.html")
      , autoindex_(false)
      , cgi_extension_(false)
      , upload_file_(false)
      , has_available_methods_(false)
      , available_methods_((struct MethodFlag){false, false, false}) {}

  tokenIterator parse_location(tokenIterator pos, tokenIterator end);
  bool          is_unavailable_method(const std::string &method) const;
};

} // namespace config

#endif /* SRCS_CONFIG_LOCATION_HPP */
