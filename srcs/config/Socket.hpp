#ifndef SRCS_CONFIG_SOCKET_HPP
#define SRCS_CONFIG_SOCKET_HPP

#include "config/Config.hpp"

class Socket {
private:
  listenFd __set_listen_fd(struct addrinfo *info);
  void     __set_bind_socket(listenFd listen_fd, struct addrinfo *info);
  void     __set_listen_passive_socket(listenFd listen_fd);

public:
  Socket() {}
  ~Socket() {}

  listenFd operator()(struct addrinfo *info) {
    listenFd listen_fd = __set_listen_fd(info);
    __set_bind_socket(listen_fd, info);
    __set_listen_passive_socket(listen_fd);
    return listen_fd;
  }
};

#endif /* SRCS_CONFIG_SOCKET_HPP */
