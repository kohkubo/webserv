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
  typedef tokenIterator (Location::*directive_location_parser_func)(
      tokenIterator pos, tokenIterator end);
  typedef std::map<std::string, directive_location_parser_func>
      directiveParserMap;
  struct MethodFlag {
    bool get_;
    bool post_;
    bool delete_;
  };

  Path               location_path_;
  Path               root_;
  Path               index_;
  bool               autoindex_;
  bool               cgi_extension_;
  bool               upload_file_;
  returnMap          return_map_;
  bool               has_available_methods_;
  MethodFlag         available_methods_;
  directiveParserMap directive_parser_map_;

public:
  Location()
      : root_("html/")
      , index_("index.html")
      , autoindex_(false)
      , cgi_extension_(false)
      , upload_file_(false)
      , has_available_methods_(false)
      , available_methods_((struct MethodFlag){false, false, false}) {}
  bool is_unavailable_method(const std::string &method) const;
  // clang-format off
  tokenIterator parse_location(tokenIterator pos, tokenIterator end);
  tokenIterator parse_root_directive(tokenIterator pos, tokenIterator end);
  tokenIterator parse_index_directive(tokenIterator pos, tokenIterator end);
  tokenIterator parse_autoindex_directive(tokenIterator pos, tokenIterator end);
  tokenIterator parse_cgi_extension_directive(tokenIterator pos, tokenIterator end);
  tokenIterator parse_upload_file_directive(tokenIterator pos, tokenIterator end);
  tokenIterator parse_available_methods_directive(tokenIterator pos, tokenIterator end);
  tokenIterator parse_location_directive(tokenIterator pos, tokenIterator end);
  tokenIterator parse_return_directive(tokenIterator pos, tokenIterator end);
  // clang-format on
};

} // namespace config

#endif /* SRCS_CONFIG_LOCATION_HPP */
