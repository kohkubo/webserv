#ifndef INCLUDES_WEBSERV_HPP
#define INCLUDES_WEBSERV_HPP

#include <map>
#include "ServerConfig.hpp"

void server_io_multiplexing(std::map<std::string, ServerConfig> &server_config_map);

#endif /* INCLUDES_WEBSERV_HPP */
