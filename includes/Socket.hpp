#ifndef INCLUDES_SOCKET_HPP
#define INCLUDES_SOCKET_HPP

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "config/ServerConfig.hpp"
#include "util.hpp"

class Socket {
public:
  explicit Socket(ServerConfig &config) : __server_config_(config) {
    set_listenfd();
    set_sockaddr_in();
    set_bind();
    set_listen();
  }
  void set_listenfd();
  void set_sockaddr_in();
  void set_bind();
  void set_listen();

  int  get_listenfd() const { return __listenfd_; }

private:
  int                __listenfd_;
  struct sockaddr_in __serv_addr_;
  ServerConfig       __server_config_;
};

#endif /* INCLUDES_SOCKET_HPP */
