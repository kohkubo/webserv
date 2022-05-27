#ifndef SRCS_UTILS_FILE_IO_UTILS_HPP
#define SRCS_UTILS_FILE_IO_UTILS_HPP

#include <string>

bool        is_path_exists(const std::string &path);
std::string read_dir_tostring(const std::string &file_path);
std::string read_file_tostring(const std::string &file_path);
bool        remove_file(const std::string &file_path);
bool        is_accessible(const std::string &file_path, int mode);

#endif /* SRCS_UTILS_FILE_IO_UTILS_HPP */
