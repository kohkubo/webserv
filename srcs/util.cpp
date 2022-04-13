#include "util.hpp"
#include <fstream>
#include <limits>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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

bool is_file_exists(const char *path) {
  struct stat file_info;

  if (stat(path, &file_info) == -1)
    return false; // exeption
  if ((file_info.st_mode & S_IFMT) == S_IFREG)
    return true;
  else
    return false;
}

std::string tostring(const std::size_t val) {
  const int max_digits = std::numeric_limits<std::size_t>::digits10 + 1;
  char      buffer[max_digits + 1];
  std::sprintf(buffer, "%zu", val);
  return buffer;
}

std::string read_file_tostring(const char *path) {
  std::ifstream file(path);
  if (!file.good()) {
    std::cout << "fail to open file" << std::endl;
    return ""; // exeption
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();
  return buffer.str();
}
