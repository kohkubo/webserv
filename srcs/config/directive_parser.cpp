#include "config/directive_parser.hpp"

#include "utils/utils.hpp"

namespace config {

tokenIterator parse_string_directive(std::string &value, tokenIterator pos,
                                     tokenIterator end) {
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    ERROR_EXIT("could not detect directive value.");
  value = *pos;
  return pos + 2;
}

tokenIterator parse_path_directive(Path &value, tokenIterator pos,
                                   tokenIterator end) {
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    ERROR_EXIT("could not detect directive value.");
  value = *pos;
  return pos + 2;
}

tokenIterator parse_size_directive(size_t &value, tokenIterator pos,
                                   tokenIterator end) {
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    ERROR_EXIT("could not detect directive value.");
  Result<std::size_t> result = string_to_size(*pos);
  if (result.is_err_)
    ERROR_EXIT("could not parse size string.");
  value = result.object_;
  return pos + 2;
}

tokenIterator parse_bool_directive(bool &value, tokenIterator pos,
                                   tokenIterator end) {
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    ERROR_EXIT("could not detect directive value.");
  if (*pos == "on")
    value = true;
  else if (*pos == "off")
    value = false;
  else
    ERROR_EXIT("bool directive value is invalid.");
  return pos + 2;
}

tokenIterator parse_map_directive(std::map<int, std::string> &value,
                                  tokenIterator pos, tokenIterator end) {
  if (pos == end || pos + 2 == end || *(pos + 2) != ";")
    ERROR_EXIT("could not detect directive value.");
  Result<std::size_t> result = string_to_size(*pos);
  if (result.is_err_) {
    ERROR_EXIT("could not detect directive value.");
  }
  value.insert(std::make_pair(result.object_, *(pos + 1)));
  return pos + 3;
}

} // namespace config
