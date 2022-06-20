#ifndef SRCS_SOCKET_SOCKETBASE_HPP
#define SRCS_SOCKET_SOCKETBASE_HPP

#include <poll.h>

#include <unistd.h>

#include "socket/SocketMapAction.hpp"
#include "utils/utils.hpp"

class SocketBase {
protected:
  int _socket_fd_;

public:
  SocketBase()
      : _socket_fd_(-1) {}
  SocketBase(int socket_fd)
      : _socket_fd_(socket_fd) {}
  virtual ~SocketBase() {
    LOG("close connection fd: " << _socket_fd_);
    close(_socket_fd_);
  }
  virtual bool            is_timed_out()                  = 0;
  virtual struct pollfd   pollfd()                        = 0;
  virtual SocketMapAction handle_event(short int revents) = 0;
  int                     socket_fd() const { return _socket_fd_; }
};

#endif /* SRCS_SOCKET_SOCKETBASE_HPP */
