#include "util.hpp"

bool is_match_suffix_string(const std::string &str, const std::string &suffix) {
  if (str.length() < suffix.length()) {
    return false;
  }
  if (str.compare(str.length() - suffix.length(), suffix.length(), suffix) ==
      0) {
    return true;
  }
  return false;
}
