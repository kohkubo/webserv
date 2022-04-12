#ifndef INCLUDES_UTIL_HPP_
#define INCLUDES_UTIL_HPP_

#include <cerrno>
#include <iostream>
#include <string.h>
#include <string>

inline void error_log_with_errno(const std::string &msg) {
  std::cerr << msg << ": " << strerror(errno) << std::endl;
}

bool is_match_suffix_string(const std::string &str, const std::string &suffix);
bool is_file_exists(const char *path);
std::string tostring(const size_t val);
std::string read_file_tostring(const char *file_path);

#endif // INCLUDES_UTIL_HPP_
