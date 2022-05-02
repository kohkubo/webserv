#ifndef SRCS_CONFIG_READ_CONFIG_HPP
#define SRCS_CONFIG_READ_CONFIG_HPP

#include "../config/ServerConfig.hpp"
#include <vector>

std::vector<ServerConfig> read_config(const char *config_file_path);

#endif /* SRCS_CONFIG_READ_CONFIG_HPP */
