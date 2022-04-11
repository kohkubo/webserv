#ifndef INCLUDES_SOCKET_HPP_
#define INCLUDES_SOCKET_HPP_

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.hpp"

class Socket {
public:
  explicit Socket(int port) : __port_(port) {
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
  int                __port_;
  struct sockaddr_in __serv_addr_;
};

#endif // INCLUDES_SOCKET_HPP_
