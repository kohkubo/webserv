#ifndef SRCS_UTILS_UTIL_HPP
#define SRCS_UTILS_UTIL_HPP

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

inline void error_log_with_errno(const std::string &msg) {
  std::cerr << msg << ": " << strerror(errno) << std::endl;
}

bool is_match_suffix_string(const std::string &str, const std::string &suffix);
bool is_file_exists(const char *path);
std::string tostring(const size_t val);
std::string read_file_tostring(const char *file_path);

template <typename T>
std::string to_string(T val) {
  std::ostringstream ss;
  ss << val;
  return ss.str();
}

#endif /* SRCS_UTILS_UTIL_HPP */
