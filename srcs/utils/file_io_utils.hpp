#ifndef SRCS_UTILS_FILE_IO_UTILS_HPP
#define SRCS_UTILS_FILE_IO_UTILS_HPP

#include <fcntl.h>

#include <string>

#include "utils/Result.hpp"

bool                is_file_exists(const std::string &path);
bool                is_dir_exists(const std::string &path);
Result<std::string> read_file_to_str(const std::string &file_path);
bool                remove_file(const std::string &file_path);
bool                is_accessible(const std::string &file_path, int mode);
Result<std::string> read_fd(int fd);
Result<int>         open_read_file(const std::string &target_path);
Result<int>         open_write_file(const std::string &target_path);

#endif /* SRCS_UTILS_FILE_IO_UTILS_HPP */
