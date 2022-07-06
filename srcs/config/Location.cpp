#include "config/Location.hpp"

#include "config/directive_parser.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

// available_methods
// 一つでも入ってたら入ってるメソッドのみ許可
// 一つもないときは全部許可

tokenIterator Location::parse_location(tokenIterator pos, tokenIterator end) {
  available_methods_.clear();
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
    pos = parse_vector_directive("available_methods", available_methods_, pos, end);
    pos = parse_bool_directive("cgi_extension", cgi_extension_, pos, end);
    pos = parse_bool_directive("upload_file", upload_file_, pos, end);
    // clang-format on
    if (pos == head) {
      ERROR_EXIT("parse location directive failed.");
    }
  }
  if (pos == end)
    ERROR_EXIT("could not detect context end.");

  if (available_methods_.size() == 0) {
    available_methods_.push_back("GET");
    available_methods_.push_back("POST");
    available_methods_.push_back("DELETE");
  }
  if (index_.has_suffix("/"))
    ERROR_EXIT("index directive failed. don't add \"/\" to file path.");
  if (!root_.has_suffix("/"))
    ERROR_EXIT("root directive failed. please add \"/\" to end of root dir");
  if (location_path_.is_minus_depth() || root_.is_minus_depth() ||
      index_.is_minus_depth())
    ERROR_EXIT("minus depth path failed.");
  return ++pos;
}
