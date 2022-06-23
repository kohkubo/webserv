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

std::size_t hexstr_to_size(const std::string &str) {
  std::size_t       x = 0;
  std::stringstream ss;
  ss << std::hex << str;
  ss >> x;
  return x;
}

bool is_dir(const std::string &filepath) {
  struct stat st = {};
  if (stat(filepath.c_str(), &st) == 0) {
    return S_ISDIR(st.st_mode);
  }
  return false;
}

bool is_minus_depth(const std::string &filepath) {
  tokenVector   tokens = tokenize(filepath, "/", "/");
  tokenIterator it     = tokens.begin();

  for (long depth = 0; it != tokens.end(); it++) {
    if (*it == "..") {
      depth--;
    } else if (*it != ".") {
      depth++;
    }
    if (depth < 0) {
      return true;
    }
  }
  return false;
}
