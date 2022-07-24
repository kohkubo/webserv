#ifndef SRCS_SOCKET_SOCKETBASE_HPP
#define SRCS_SOCKET_SOCKETBASE_HPP

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include "socket/Timeout.hpp"
#include "utils/utils.hpp"

namespace ns_socket {

class SocketMapActions;

struct ReceiveResult {
  enum ReceiveStatus {
    ERROR = -1,
    END,
    SUCCESS,
  };

  ReceiveStatus status_;
  std::string   str_;

  ReceiveResult(ReceiveStatus status, std::string str)
      : status_(status)
      , str_(str) {}
};

class SocketBase {
protected:
  int     _socket_fd_;
  Timeout _timeout_;

public:
  SocketBase()
      : _socket_fd_(-1) {}
  SocketBase(int socket_fd, const std::time_t timeout_seconds)
      : _socket_fd_(socket_fd)
      , _timeout_(timeout_seconds) {
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
  virtual SocketMapActions destroy_timedout_socket()       = 0;
  virtual struct pollfd    pollfd()                        = 0;
  virtual SocketMapActions handle_event(short int revents) = 0;
  int                      socket_fd() const { return _socket_fd_; }
  ReceiveResult            receive_content(size_t size) const;
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_SOCKETBASE_HPP */
