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
  connFd                  __conn_fd_;
  confGroup               __conf_group_;
  std::deque<Transaction> __transaction_queue_;
  std::string             __buffer_;

private:
  void __check_buffer_length_exception();

public:
  Connection() {}
  Connection(connFd conn_fd, confGroup conf_group)
      : __conn_fd_(conn_fd)
      , __conf_group_(conf_group) {
    __transaction_queue_.push_back(Transaction(conn_fd));
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
};

#endif /* SRCS_EVENT_CONNECTION_HPP */
