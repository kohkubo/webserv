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

bool        has_suffix(const std::string &str, const std::string &suffix);
bool        has_prefix(const std::string &str, const std::string &prefix);
std::string tolower(const std::string &str);

template <typename T>
std::string to_string(T val) {
  std::ostringstream ss;
  ss << val;
  return ss.str();
}

bool        is_uint8(const std::string &str);
bool        is_ip(const std::string &str);
bool        is_digits(const std::string &str);

std::size_t hexstr_to_size(const std::string &str);

#endif /* SRCS_UTILS_UTILS_HPP */
