#ifndef SRCS_EVENT_CONNECTION_HPP
#define SRCS_EVENT_CONNECTION_HPP

#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <ctime>
#include <deque>
#include <string>
#include <vector>

#include "event/Request.hpp"
#include "event/Response.hpp"

// TODO: string -> vector<char>

typedef int connFd;

class Connection {
private:
  connFd                   _conn_fd_;
  confGroup                _conf_group_;
  Request                  _request_;
  std::deque<Response>     _response_queue_;
  std::string              _buffer_;
  std::time_t              _last_event_time_;
  static const std::time_t TIMEOUT_SECONDS_ = 60;

private:
  Connection() {}
  static std::time_t _time_now() { return std::time(NULL); }

public:
  Connection(connFd conn_fd, confGroup conf_group)
      : _conn_fd_(conn_fd)
      , _conf_group_(conf_group)
      , _last_event_time_(_time_now()) {}
  ~Connection() {}

  connFd        conn_fd() const { return _conn_fd_; }
  void          create_sequential_transaction();
  struct pollfd create_pollfd() const;
  bool          append_receive_buffer();
  void          send_front_response();
  bool          is_timed_out() const {
             return std::difftime(_time_now(), _last_event_time_) >= TIMEOUT_SECONDS_;
  }
  void close() const { ::close(_conn_fd_); }
};

#endif /* SRCS_EVENT_CONNECTION_HPP */
