#ifndef SRCS_SOCKET_SOCKETBASE_HPP
#define SRCS_SOCKET_SOCKETBASE_HPP

#include <poll.h>

#include "socket/SocketMapOp.hpp"

class SocketBase {
protected:
  int _socket_fd_;

public:
  SocketBase()
      : _socket_fd_(-1) {}
  SocketBase(int socket_fd)
      : _socket_fd_(socket_fd) {}
  virtual ~SocketBase() {}
  virtual bool          is_timed_out()                  = 0;
  virtual struct pollfd pollfd()                        = 0;
  virtual SocketMapOp   handle_event(short int revents) = 0;
};

#endif /* SRCS_SOCKET_SOCKETBASE_HPP */
