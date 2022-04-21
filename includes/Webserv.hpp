#ifndef INCLUDES_WEBSERV_HPP
#define INCLUDES_WEBSERV_HPP

#include "ServerConfig.hpp"

ServerConfig read_config_file(const char *config_file_path);
void server_io_multiplexing(const ServerConfig &server_config);

#endif /* INCLUDES_WEBSERV_HPP */
