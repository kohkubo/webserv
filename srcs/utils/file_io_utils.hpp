#ifndef SRCS_UTILS_FILE_IO_UTILS_HPP
#define SRCS_UTILS_FILE_IO_UTILS_HPP

#include <string>

bool        is_file_exists(const char *path);
std::string read_file_tostring(const char *file_path);

#endif /* SRCS_UTILS_FILE_IO_UTILS_HPP */
