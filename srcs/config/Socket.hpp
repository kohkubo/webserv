#ifndef SRCS_CONFIG_SOCKET_HPP
#define SRCS_CONFIG_SOCKET_HPP

#include "config/Config.hpp"

class Socket {
 public:
  Socket(struct addrinfo *info) : __info_(info) {
    __set_listen_fd();
    __set_bind_socket();
    __set_listen_passive_socket();
  };
  ~Socket(){};
  listenFd get_listen_fd() { return __listen_fd_; };

 private:
  Socket(const Socket &other);
  Socket &operator=(const Socket &other);
  struct addrinfo *__info_;
  listenFd __listen_fd_;
  void __set_listen_fd();
  void __set_bind_socket();
  void __set_listen_passive_socket();
};

#endif /* SRCS_CONFIG_SOCKET_HPP */
