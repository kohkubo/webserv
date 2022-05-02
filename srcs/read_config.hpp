#ifndef SRCS_READ_CONFIG_HPP
#define SRCS_READ_CONFIG_HPP

#include "ServerConfig.hpp"
#include <vector>

std::vector<ServerConfig> read_config(const char *config_file_path);

#endif /* SRCS_READ_CONFIG_HPP */
