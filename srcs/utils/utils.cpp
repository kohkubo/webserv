#include "utils/utils.hpp"

#include <sys/stat.h>

#include <algorithm>
#include <cctype>

#include "utils/tokenize.hpp"

bool has_suffix(const std::string &str, const std::string &suffix) {
  if (str.length() < suffix.length()) {
    return false;
  }
  if (str.compare(str.length() - suffix.length(), suffix.length(), suffix) ==
      0) {
    return true;
  }
  return false;
}

bool has_prefix(const std::string &str, const std::string &prefix) {
  std::string::size_type size = prefix.size();
  if (str.size() < size)
    return false;
  return std::equal(prefix.begin(), prefix.end(), str.begin());
}

static char my_tolower(char ch) {
  return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}

std::string tolower(const std::string &str) {
  std::string lower = str;
  std::transform(lower.begin(), lower.end(), lower.begin(), my_tolower);
  return lower;
}

bool is_uint8(const std::string &str) {
  if (str.size() == 1) {
    if (str[0] >= '0' && str[0] <= '9') {
      return true;
    }
    return false;
  } else if (str.size() == 2) {
    if (str[0] >= '1' && str[0] <= '9' && str[1] >= '0' && str[1] <= '9') {
      return true;
    }
    return false;
  } else if (str.size() == 3) {
    if (str[0] == '1') {
      if ((str[1] >= '0' && str[1] <= '9') && (str[2] >= '0' && str[2] <= '9'))
        return true;
    }
    if (str[0] == '2') {
      if ((str[1] >= '0' && str[1] <= '4') && (str[2] >= '0' && str[2] <= '9'))
        return true;
      if (str[1] == '5' && (str[2] >= '0' && str[2] <= '5'))
        return true;
    }
  }
  return false;
}

bool is_ip(const std::string &str) {
  tokenVector   tokens = tokenize(str, ".", "");
  tokenIterator it     = tokens.begin();
  size_t        count  = 0;
  for (; it != tokens.end(); it++, count++) {
    if (!is_uint8(*it))
      return false;
    it++;
    if (count == 3)
      break;
    if (it != tokens.end() && *it != ".")
      return false;
  }
  return true;
}

bool is_digits(const std::string &str) {
  for (size_t i = 0; i < str.size(); i++) {
    if (!isdigit(str[i]))
      return false;
  }
  return true;
}

bool is_dir(const std::string &filepath) {
  struct stat st;
  if (stat(filepath.c_str(), &st) == 0) {
    return S_ISDIR(st.st_mode);
  }
  return false;
}