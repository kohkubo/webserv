#ifndef SRCS_UTILS_FILE_IO_UTILS_HPP
#define SRCS_UTILS_FILE_IO_UTILS_HPP

#include <string>

struct RetPair {
  bool        is_err_;
  std::string str_;
};

bool    is_file_exists(const std::string &path);
bool    is_dir_exists(const std::string &path);
RetPair read_file_to_str(const std::string &file_path);
bool    remove_file(const std::string &file_path);
bool    is_accessible(const std::string &file_path, int mode);

#endif /* SRCS_UTILS_FILE_IO_UTILS_HPP */
