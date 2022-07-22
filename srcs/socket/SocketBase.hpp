#ifndef SRCS_SOCKET_SOCKETBASE_HPP
#define SRCS_SOCKET_SOCKETBASE_HPP

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include "utils/utils.hpp"

namespace ns_socket {

class SocketMapActions;

class SocketBase {
protected:
  int _socket_fd_;

public:
  SocketBase()
      : _socket_fd_(-1) {}
  SocketBase(int socket_fd)
      : _socket_fd_(socket_fd) {
    if (fcntl(_socket_fd_, F_SETFL, O_NONBLOCK) == -1) {
      ERROR_EXIT_WITH_ERRNO("fcntl() failed.");
    }
  }
  virtual ~SocketBase() {
    // LOG("close connection fd: " << _socket_fd_);
    if (close(_socket_fd_) == -1) {
      ERROR_LOG_WITH_ERRNO("close connection fd: " << _socket_fd_);
    }
  }
  virtual bool             is_timed_out()                  = 0;
  virtual SocketMapActions handle_timed_out()              = 0;
  virtual struct pollfd    pollfd()                        = 0;
  virtual SocketMapActions handle_event(short int revents) = 0;
  int                      socket_fd() const { return _socket_fd_; }
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_SOCKETBASE_HPP */
