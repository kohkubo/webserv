#ifndef SRCS_CONFIG_CONFIG_HPP
#define SRCS_CONFIG_CONFIG_HPP

#include "config/Config.hpp"

int         open_new_socket(const ServerConfig &config);
server_list read_config(const char *config_file_path);
std::map<listen_fd, conf_group>
create_socket_map(const server_list &server_list);

#endif /* SRCS_CONFIG_CONFIG_HPP */
