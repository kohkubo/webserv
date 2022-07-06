#include "config/Location.hpp"

#include "config/directive_parser.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

tokenIterator Location::_parse_available_methods(tokenIterator pos,
                                                 tokenIterator end) {
  if (*pos != "available_methods")
    return pos;
  pos++;
  if (pos == end)
    ERROR_EXIT("could not detect directive value.");
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
  if (pos == end)
    ERROR_EXIT("available methods directive value is invalid.");
  return pos + 1;
}

tokenIterator Location::parse_location(tokenIterator pos, tokenIterator end) {
  pos++;
  if (pos == end)
    ERROR_EXIT("could not detect directive value.");
  location_path_ = *pos++;
  if (pos == end || *pos != "{")
    ERROR_EXIT("could not detect context.");
  pos++;
  while (pos != end && *pos != "}") {
    tokenIterator head = pos;
    // clang-format off
    pos = parse_path_directive("root", root_, pos, end);
    pos = parse_path_directive("index", index_, pos, end);
    pos = parse_bool_directive("autoindex", autoindex_, pos, end);
    pos = parse_map_directive("return", return_map_, pos, end);
    pos = _parse_available_methods(pos, end);
    pos = parse_bool_directive("cgi_extension", cgi_extension_, pos, end);
    pos = parse_bool_directive("upload_file", upload_file_, pos, end);
    // clang-format on
    if (pos == head) {
      ERROR_EXIT("parse location directive failed.");
    }
  }
  if (pos == end)
    ERROR_EXIT("could not detect context end.");
  if (index_.has_suffix("/"))
    ERROR_EXIT("index directive failed. don't add \"/\" to file path.");
  if (!root_.has_suffix("/"))
    ERROR_EXIT("root directive failed. please add \"/\" to end of root dir");
  if (location_path_.is_minus_depth() || root_.is_minus_depth() ||
      index_.is_minus_depth())
    ERROR_EXIT("minus depth path failed.");
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