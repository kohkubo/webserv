#ifndef SRCS_CONFIG_SOCKETOPENER_HPP
#define SRCS_CONFIG_SOCKETOPENER_HPP

#include "config/Config.hpp"

class SocketOpener {
private:
  listenFd __set_listen_fd(struct addrinfo *info);
  void     __set_bind_socket(listenFd listen_fd, struct addrinfo *info);
  void     __set_listen_passive_socket(listenFd listen_fd);

public:
  SocketOpener() {}
  ~SocketOpener() {}

  listenFd operator()(struct addrinfo *info) {
    listenFd listen_fd = __set_listen_fd(info);
    __set_bind_socket(listen_fd, info);
    __set_listen_passive_socket(listen_fd);
    return listen_fd;
  }
};

#endif /* SRCS_CONFIG_SOCKETOPENER_HPP */
