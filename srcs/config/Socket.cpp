#include "config/Socket.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>

#include "config/Config.hpp"
#include "utils/utils.hpp"

listenFd Socket::__set_listen_fd(struct addrinfo *info) {
  listenFd listen_fd =
      socket(info->ai_family, info->ai_socktype, info->ai_protocol);
  if (listen_fd == -1) {
    error_log_with_errno("socket() failed.");
    exit(EXIT_FAILURE);
  }
  if (fcntl(listen_fd, F_SETFL, O_NONBLOCK) == -1) {
    error_log_with_errno("fcntl() failed.");
    exit(EXIT_FAILURE);
  }
  int on = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&on,
                 sizeof(on)) == -1) {
    error_log_with_errno("setsockopt() failed.");
    close(listen_fd);
    exit(EXIT_FAILURE);
  }
  return listen_fd;
}

void Socket::__set_listen_passive_socket(listenFd listen_fd) {
  if (listen(listen_fd, SOMAXCONN) == -1) {
    error_log_with_errno("listen() failed.");
    close(listen_fd);
    exit(EXIT_FAILURE);
  }
}

void Socket::__set_bind_socket(listenFd listen_fd, struct addrinfo *info) {
  if (bind(listen_fd, info->ai_addr, info->ai_addrlen) == -1) {
    error_log_with_errno("bind() failed.");
    close(listen_fd);
    exit(EXIT_FAILURE);
  }
}
