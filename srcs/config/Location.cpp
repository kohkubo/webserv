#include "config/Location.hpp"

#include "config/directive_parser.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

namespace config {

static Location::directiveParserMap init_directive_parser_map() {
  Location::directiveParserMap directive_parser_map;
  // clang-format off
  directive_parser_map["root"] = &Location::parse_root_directive;
  directive_parser_map["index"] = &Location::parse_index_directive;
  directive_parser_map["autoindex"] = &Location::parse_autoindex_directive;
  directive_parser_map["cgi_extension"] = &Location::parse_cgi_extension_directive;
  directive_parser_map["upload_file"] = &Location::parse_upload_file_directive;
  directive_parser_map["available_methods"] = &Location::parse_available_methods_directive;
  directive_parser_map["return"] = &Location::parse_return_directive;
  // clang-format on
  return directive_parser_map;
}

Location::directiveParserMap g_directive_location_parser_map =
    init_directive_parser_map();

tokenIterator Location::parse_location(tokenIterator pos, tokenIterator end) {
  if (pos == end)
    ERROR_EXIT("could not detect directive value.");
  location_path_ = *pos++;
  if (location_path_.is_minus_depth()) {
    ERROR_EXIT("minus depth path failed.");
  }
  if (pos == end || *pos != "{") {
    ERROR_EXIT("could not detect context. " + *pos);
  }
  pos++;
  while (pos != end && *pos != "}") {
    directiveParserMap::iterator it =
        g_directive_location_parser_map.find(*pos);
    if (it == g_directive_location_parser_map.end()) {
      ERROR_EXIT("unknown location directive: " + *pos);
    }
    pos = (this->*it->second)(++pos, end);
  }
  if (pos == end)
    ERROR_EXIT("could not detect context end.");
  return ++pos;
}

bool Location::is_unavailable_method(const std::string &method) const {
  if (!has_available_methods_) {
    return false;
  }
  if (method == "GET") {
    return !available_methods_.get_;
  }
  if (method == "POST") {
    return !available_methods_.post_;
  }
  if (method == "GET") {
    return !available_methods_.delete_;
  }
  return true;
}

tokenIterator Location::parse_root_directive(tokenIterator pos,
                                             tokenIterator end) {
  tokenIterator token_iteratoer = parse_path_directive(root_, pos, end);
  if (!root_.has_suffix("/")) {
    ERROR_EXIT("root directive failed. please add \"/\" to end of root dir");
  }
  if (root_.is_minus_depth()) {
    ERROR_EXIT("minus depth path failed.");
  }
  return token_iteratoer;
}

tokenIterator Location::parse_index_directive(tokenIterator pos,
                                              tokenIterator end) {
  tokenIterator token_iteratoer = parse_path_directive(index_, pos, end);
  if (index_.has_suffix("/")) {
    ERROR_EXIT("index directive failed. don't add \"/\" to file path.");
  }
  if (index_.is_minus_depth()) {
    ERROR_EXIT("minus depth path failed.");
  }
  return token_iteratoer;
}

tokenIterator Location::parse_autoindex_directive(tokenIterator pos,
                                                  tokenIterator end) {
  return parse_bool_directive(autoindex_, pos, end);
}

tokenIterator Location::parse_return_directive(tokenIterator pos,
                                               tokenIterator end) {
  return parse_map_directive(return_map_, pos, end);
}

tokenIterator Location::parse_available_methods_directive(tokenIterator pos,
                                                          tokenIterator end) {
  if (pos == end) {
    ERROR_EXIT("could not detect directive value.");
  }
  has_available_methods_ = true;
  for (; pos != end && *pos != ";"; pos++) {
    if (*pos == "GET") {
      available_methods_.get_ = true;
    } else if (*pos == "POST") {
      available_methods_.post_ = true;
    } else if (*pos == "DELETE") {
      available_methods_.delete_ = true;
    }
  }
  if (pos == end) {
    ERROR_EXIT("available methods directive value is invalid.");
  }
  return pos + 1;
}

tokenIterator Location::parse_cgi_extension_directive(tokenIterator pos,
                                                      tokenIterator end) {
  return parse_bool_directive(cgi_extension_, pos, end);
}

tokenIterator Location::parse_upload_file_directive(tokenIterator pos,
                                                    tokenIterator end) {
  return parse_bool_directive(upload_file_, pos, end);
}

} // namespace config