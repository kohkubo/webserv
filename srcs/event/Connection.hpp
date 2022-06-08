#ifndef SRCS_EVENT_CONNECTION_HPP
#define SRCS_EVENT_CONNECTION_HPP

#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <ctime>
#include <deque>
#include <string>
#include <vector>

#include "event/Transaction.hpp"

// TODO: string -> vector<char>

typedef class Config Config;
typedef int          connFd;

class Connection {
private:
  connFd                   __conn_fd_;
  confGroup                __conf_group_;
  std::deque<Transaction>  __transaction_queue_;
  std::string              __buffer_;
  std::time_t              __last_event_time_;
  static const std::time_t timeout_seconds_ = 60;

private:
  Connection() {}
  static std::time_t __time_now() { return std::time(NULL); }

public:
  Connection(connFd conn_fd, confGroup conf_group)
      : __conn_fd_(conn_fd)
      , __conf_group_(conf_group) {
    __transaction_queue_.push_back(Transaction());
    __last_event_time_ = __time_now();
  }
  ~Connection() {}

  void          create_sequential_transaction();
  struct pollfd create_pollfd() const;
  bool          append_receive_buffer();
  void          send_response();
  void          shutdown_write() {
    shutdown(__conn_fd_, SHUT_WR);
    __transaction_queue_.front().set_transaction_state(CLOSING);
  }

  bool is_timed_out() const {
    std::time_t now = std::time(NULL);
    return std::difftime(now, __last_event_time_) >= timeout_seconds_;
  }
};

#endif /* SRCS_EVENT_CONNECTION_HPP */
