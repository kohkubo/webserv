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
  Socket(ServerConfig &config);
  ~Socket();
  int get_listenfd() const { return __listenfd_; }

private:
  Socket &operator=(const Socket &other);
  Socket(const Socket &other);
  void               __set_listenfd();
  void               __set_sockaddr_in();
  void               __set_bind();
  void               __set_listen();

  int                __port_;
  int                __listenfd_;
  struct sockaddr_in __serv_addr_;
  ServerConfig       __server_config_;
};

#endif /* INCLUDES_SOCKET_HPP */
