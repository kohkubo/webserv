#ifndef SRCS_UTILS_FILE_IO_UTILS_HPP
#define SRCS_UTILS_FILE_IO_UTILS_HPP

#include <string>

struct Result {
  bool        is_err_;
  std::string str_;

  Result(bool is_err = false, const std::string &str = "")
      : is_err_(is_err)
      , str_(str) {}
};

bool   is_file_exists(const std::string &path);
bool   is_dir_exists(const std::string &path);
Result read_file_to_str(const std::string &file_path);
bool   remove_file(const std::string &file_path);
bool   is_accessible(const std::string &file_path, int mode);

#endif /* SRCS_UTILS_FILE_IO_UTILS_HPP */
