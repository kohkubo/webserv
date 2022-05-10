#ifndef SRCS_CONFIG_CONFIG_HPP
#define SRCS_CONFIG_CONFIG_HPP

#include "config/ServerConfig.hpp"

server_list_type  read_config(const char *config_file_path);
server_group_type create_server_group(const server_list_type &server_list);

#endif /* SRCS_CONFIG_CONFIG_HPP */
