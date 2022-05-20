#ifndef SRCS_CONFIG_CONFIG_HPP
#define SRCS_CONFIG_CONFIG_HPP

#include "config/Config.hpp"

int                            open_new_socket(const Config &config);
serverList                     read_config(const char *config_file_path);
std::map<listenFd, conf_group> create_socket_map(const serverList &server_list);

#endif /* SRCS_CONFIG_CONFIG_HPP */
