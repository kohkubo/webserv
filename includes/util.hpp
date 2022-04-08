#ifndef INCLUDES_UTIL_HPP_
#define INCLUDES_UTIL_HPP_

#include <cerrno>
#include <iostream>
#include <string>

inline void error_log_with_errno(const std::string &msg) {
  std::cerr << msg << ": " << std::strerror(errno) << std::endl;
}

bool is_match_suffix_string(const std::string &str, const std::string &suffix);

#endif // INCLUDES_UTIL_HPP_
