#ifndef SRCS_UTILS_FILE_IO_UTILS_HPP
#define SRCS_UTILS_FILE_IO_UTILS_HPP

#include <fcntl.h>

#include <string>

#include "utils/Result.hpp"

Result<std::string> read_fd(int fd);

#endif /* SRCS_UTILS_FILE_IO_UTILS_HPP */
