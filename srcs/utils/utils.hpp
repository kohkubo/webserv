#ifndef SRCS_UTILS_UTILS_HPP
#define SRCS_UTILS_UTILS_HPP

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

template <typename T>
std::string to_string(T val) {
  std::ostringstream ss;
  ss << val;
  return ss.str();
}

#endif /* SRCS_UTILS_UTILS_HPP */
