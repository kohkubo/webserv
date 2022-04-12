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
std::string read_file_to_string(const std::string &path);
std::string sizet_to_string(size_t val);

#endif // INCLUDES_UTIL_HPP_
