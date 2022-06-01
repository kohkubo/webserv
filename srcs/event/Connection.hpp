#ifndef SRCS_EVENT_CONNECTION_HPP
#define SRCS_EVENT_CONNECTION_HPP

#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

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
  std::string              __response_;
  ssize_t                  __response_string_size_;
  static const std::size_t buffer_max_length_ = 8192;

private:
  Connection() {}
  static void __check_buffer_length_exception(std::string &request_buffer,
                                              std::size_t  buffer_limit_length);
  static bool __is_send_all(ssize_t            response_string_size,
                            const std::string &response) {
    return response_string_size == static_cast<ssize_t>(response.size());
  }

public:
  Connection(connFd conn_fd, confGroup conf_group)
      : __conn_fd_(conn_fd)
      , __conf_group_(conf_group) {
    __transaction_queue_.push_back(Transaction());
  }
  ~Connection() {}

  void          create_sequential_transaction();
  struct pollfd create_pollfd() const;
  bool          append_receive_buffer();
  void          send_response(connFd conn_fd);
};

#endif /* SRCS_EVENT_CONNECTION_HPP */
