#ifndef SRCS_SOCKET_SOCKETBASE_HPP
#define SRCS_SOCKET_SOCKETBASE_HPP

#include <poll.h>

class SocketBase {
protected:
  int _socket_fd_;

public:
  SocketBase()
      : _socket_fd_(-1) {}
  virtual ~SocketBase();
  virtual struct pollfd pollfd()                        = 0;
  virtual void          handle_event(short int revents) = 0;
};

#endif /* SRCS_SOCKET_SOCKETBASE_HPP */
