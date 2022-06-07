#ifndef SRCS_CONFIG_SOCKETOPENER_HPP
#define SRCS_CONFIG_SOCKETOPENER_HPP

#include "config/Config.hpp"

class SocketOpener {
private:
  SocketOpener() {}
  ~SocketOpener() {}
  static listenFd __create_socket(struct addrinfo *info);
  static void     __bind_address(listenFd listen_fd, struct addrinfo *info);
  static void     __start_listen(listenFd listen_fd);

public:
  static listenFd open_new_socket(struct addrinfo *info) {
    listenFd listen_fd = __create_socket(info);
    __bind_address(listen_fd, info);
    __start_listen(listen_fd);
    return listen_fd;
  }
};

#endif /* SRCS_CONFIG_SOCKETOPENER_HPP */
