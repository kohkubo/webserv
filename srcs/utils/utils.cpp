#include "utils/utils.hpp"

#include <algorithm>
#include <cctype>

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

bool starts_with(const std::string &s, const std::string &prefix) {
  std::string::size_type size = prefix.size();
  if (s.size() < size)
    return false;
  return std::equal(prefix.begin(), prefix.end(), s.begin());
}

static char my_tolower(char ch) {
  return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}

std::string tolower(const std::string &str) {
  std::string lower = str;
  std::transform(lower.begin(), lower.end(), lower.begin(), my_tolower);
  return lower;
}