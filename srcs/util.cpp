#include "util.hpp"
#include <fstream>
#include <limits>
#include <sstream>

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

std::string sizet_to_string(std::size_t val) {
  const int max_digits = std::numeric_limits<std::size_t>::digits10 + 1;
  char      buffer[max_digits + 1];
  std::sprintf(buffer, "%zu", val);
  return buffer;
}

std::string read_file_to_string(const std::string &path) {
  std::ifstream file(path.c_str());
  if (!file.good()) {
    std::cout << "fail to open file" << std::endl;
    return "file not found"; // -> 404 or 500
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();
  return buffer.str();
}
