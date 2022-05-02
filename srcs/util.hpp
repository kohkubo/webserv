#ifndef SRCS_UTIL_HPP
#define SRCS_UTIL_HPP

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
std::string              tostring(const size_t val);
std::string              read_file_tostring(const char *file_path);

bool                     is_ip(const std::string &token);
bool                     is_uint8(const std::string &token);
bool                     is_digits(const std::string &token);

std::vector<std::string> tokenize(const std::string &text,
                                  const std::string &delimiter,
                                  const std::string &skip);

template <typename T>
std::string to_string(T val) {
  std::ostringstream ss;
  ss << val;
  return ss.str();
}

#endif /* SRCS_UTIL_HPP */
