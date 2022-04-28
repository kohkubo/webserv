#ifndef INCLUDES_SOCKET_HPP
#define INCLUDES_SOCKET_HPP

#include <cstdlib>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "ServerConfig.hpp"
#include "util.hpp"

int              open_new_socket(const ServerConfig &config);
struct addrinfo *getaddrinfo_via_serv(const ServerConfig &conf);

#endif /* INCLUDES_SOCKET_HPP */
