#ifndef INCLUDES_UTIL_HPP
#define INCLUDES_UTIL_HPP

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

inline void error_log_with_errno(const std::string &msg) {
  std::cerr << msg << ": " << strerror(errno) << std::endl;
}

bool is_match_suffix_string(const std::string &str, const std::string &suffix);
bool is_file_exists(const char *path);
std::string tostring(const size_t val);
std::string read_file_tostring(const char *file_path);

bool        is_ip(const std::string &token);
bool        is_uint8(const std::string &token);
bool        is_digits(const std::string &token);

template <typename T>
std::string to_string(T val) {
  std::ostringstream ss;
  ss << val;
  return ss.str();
  // valが空のifstream: ss.fail() -> true, ss.str() = ""
  // 問題ない. その他のエラーケースも再現が困難なので現状エラー処理なし.
}

#endif /* INCLUDES_UTIL_HPP */
