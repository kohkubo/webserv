#include "config/SocketOpener.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>

#include "config/Config.hpp"
#include "utils/utils.hpp"

listenFd SocketOpener::__create_socket(struct addrinfo *info) {
  listenFd listen_fd =
      socket(info->ai_family, info->ai_socktype, info->ai_protocol);
  if (listen_fd == -1) {
    ERROR_LOG_WITH_ERRNO("socket() failed.");
    exit(EXIT_FAILURE);
  }
  if (fcntl(listen_fd, F_SETFL, O_NONBLOCK) == -1) {
    ERROR_LOG_WITH_ERRNO("fcntl() failed.");
    exit(EXIT_FAILURE);
  }
  int on = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&on,
                 sizeof(on)) == -1) {
    ERROR_LOG_WITH_ERRNO("setsockopt() failed.");
    close(listen_fd);
    exit(EXIT_FAILURE);
  }
  return listen_fd;
}

void SocketOpener::__bind_address(listenFd listen_fd, struct addrinfo *info) {
  if (bind(listen_fd, info->ai_addr, info->ai_addrlen) == -1) {
    ERROR_LOG_WITH_ERRNO("bind() failed.");
    close(listen_fd);
    exit(EXIT_FAILURE);
  }
}

void SocketOpener::__start_listen(listenFd listen_fd) {
  if (listen(listen_fd, SOMAXCONN) == -1) {
    ERROR_LOG_WITH_ERRNO("listen() failed.");
    close(listen_fd);
    exit(EXIT_FAILURE);
  }
}
