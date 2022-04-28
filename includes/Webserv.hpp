#ifndef INCLUDES_WEBSERV_HPP
#define INCLUDES_WEBSERV_HPP

#include "ServerConfig.hpp"
#include <vector>

std::vector<ServerConfig> read_config(const char *config_file_path);
void server_io_multiplexing(const ServerConfig &server_config);

#endif /* INCLUDES_WEBSERV_HPP */