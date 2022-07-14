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
  typedef tokenIterator (Location::*directive_location_parser_func)(
      tokenIterator pos, tokenIterator end);
  typedef std::map<std::string, directive_location_parser_func>
      directiveParserMap;

public:
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
      , available_methods_((struct MethodFlag){false, false, false}) {
    _init_directive_parser_map();
  }

  bool          is_unavailable_method(const std::string &method) const;
  tokenIterator parse_location(tokenIterator pos, tokenIterator end);

private:
  // clang-format off
  tokenIterator _parse_root_directive(tokenIterator pos, tokenIterator end);
  tokenIterator _parse_index_directive(tokenIterator pos, tokenIterator end);
  tokenIterator _parse_autoindex_directive(tokenIterator pos, tokenIterator end);
  tokenIterator _parse_cgi_extension_directive(tokenIterator pos, tokenIterator end);
  tokenIterator _parse_upload_file_directive(tokenIterator pos, tokenIterator end);
  tokenIterator _parse_available_methods_directive(tokenIterator pos, tokenIterator end);
  tokenIterator _parse_location_directive(tokenIterator pos, tokenIterator end);
  tokenIterator _parse_return_directive(tokenIterator pos, tokenIterator end);
  // clang-format on
  void _init_directive_parser_map();
};

} // namespace config

#endif /* SRCS_CONFIG_LOCATION_HPP */
