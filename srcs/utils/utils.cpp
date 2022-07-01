#include "utils/utils.hpp"

#include <sys/stat.h>

#include <algorithm>
#include <cctype>

#include "utils/tokenize.hpp"

bool has_suffix(const std::string &str, const std::string &suffix) {
  if (str.length() < suffix.length()) {
    return false;
  }
  return (str.compare(str.length() - suffix.length(), suffix.length(),
                      suffix) == 0);
}

bool has_prefix(const std::string &str, const std::string &prefix) {
  std::string::size_type size = prefix.size();
  if (str.size() < size)
    return false;
  return std::equal(prefix.begin(), prefix.end(), str.begin());
}

std::string trim(std::string str, std::string cutset) {
  str.erase(0, str.find_first_not_of(cutset));
  std::size_t pos = str.find_last_not_of(cutset);
  if (pos != std::string::npos) {
    str.erase(pos + 1);
  }
  return str;
}

static char my_tolower(char ch) {
  return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}

std::string tolower(const std::string &str) {
  std::string lower = str;
  std::transform(lower.begin(), lower.end(), lower.begin(), my_tolower);
  return lower;
}

bool getline(std::string &source, std::string &line) {
  std::size_t pos = source.find("\r\n");
  if (pos == std::string::npos)
    return false;
  line = source.substr(0, pos);
  source.erase(0, pos + 2);
  return true;
}

Result<std::size_t> string_to_size(const std::string &str) {
  for (size_t i = 0; i < str.size(); i++) {
    char c = str[i];
    if (!(std::isdigit(c) != 0)) {
      return Error<std::size_t>();
    }
  }

  std::size_t       result = 0;
  std::stringstream ss(str);
  ss >> result;
  return Ok<std::size_t>(result);
}

Result<std::size_t> hexstr_to_size(const std::string &str) {
  for (size_t i = 0; i < str.size(); i++) {
    char c = str[i];
    if (!(std::isdigit(c) != 0 || ('a' <= c && c <= 'f') ||
          ('A' <= c && c <= 'F'))) {
      return Error<std::size_t>();
    }
  }

  std::size_t       result = 0;
  std::stringstream ss;
  ss << std::hex << str;
  ss >> result;
  return Ok<std::size_t>(result);
}
