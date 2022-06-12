#ifndef SRCS_EVENT_CONNECTION_HPP
#define SRCS_EVENT_CONNECTION_HPP

#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <ctime>
#include <deque>
#include <string>
#include <vector>

#include "event/ReceiveRequest.hpp"
#include "event/SendResponse.hpp"

// TODO: string -> vector<char>

typedef class Config Config;
typedef int          connFd;

class Connection {
private:
  connFd                   __conn_fd_;
  confGroup                __conf_group_;
  ReceiveRequest           __receive_request_;
  std::deque<SendResponse> __response_queue_;
  std::string              __buffer_;
  std::time_t              __last_event_time_;
  static const std::time_t timeout_seconds_ = 60;

private:
  Connection() {}
  static std::time_t __time_now() { return std::time(NULL); }

public:
  Connection(connFd conn_fd, confGroup conf_group)
      : __conn_fd_(conn_fd)
      , __conf_group_(conf_group)
      , __last_event_time_(__time_now()) {}
  ~Connection() {}

  void          create_sequential_transaction();
  struct pollfd create_pollfd() const;
  bool          append_receive_buffer();
  void          send_front_response();
  bool          is_timed_out() const {
    return std::difftime(__time_now(), __last_event_time_) >= timeout_seconds_;
  }
};

#endif /* SRCS_EVENT_CONNECTION_HPP */
