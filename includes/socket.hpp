#ifndef INCLUDES_SOCKET_HPP
#define INCLUDES_SOCKET_HPP

#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "ServerConfig.hpp"
#include "util.hpp"

int open_new_socket(const ServerConfig &config);

#endif /* INCLUDES_SOCKET_HPP */
