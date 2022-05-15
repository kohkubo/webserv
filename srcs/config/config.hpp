#ifndef SRCS_CONFIG_CONFIG_HPP
#define SRCS_CONFIG_CONFIG_HPP

#include "config/ServerConfig.hpp"

server_list_type read_config(const char *config_file_path);
socket_list_type create_socket_map(const server_list_type &server_list);

#endif /* SRCS_CONFIG_CONFIG_HPP */
