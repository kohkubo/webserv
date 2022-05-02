#ifndef SRCS_SOCKET_HPP
#define SRCS_SOCKET_HPP

#include "../config/ServerConfig.hpp"
#include "../util/util.hpp"

int              open_new_socket(const ServerConfig &config);
struct addrinfo *getaddrinfo_via_serv(const ServerConfig &conf);

#endif /* SRCS_SOCKET_HPP */
