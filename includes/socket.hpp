#ifndef INCLUDES_SOCKET_HPP
#define INCLUDES_SOCKET_HPP

#include "ServerConfig.hpp"
#include "util.hpp"

int              open_new_socket(const ServerConfig &config);
struct addrinfo *getaddrinfo_via_serv(const ServerConfig &conf);

#endif /* INCLUDES_SOCKET_HPP */
