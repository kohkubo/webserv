#ifndef SRCS_EVENT_CONNECTION_HPP
#define SRCS_EVENT_CONNECTION_HPP

#include <poll.h>
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

public:
  Connection() {}
  Connection(connFd conn_fd, confGroup conf_group)
      : __conn_fd_(conn_fd)
      , __conf_group_(conf_group) {
    __transaction_queue_.push_back(Transaction(conn_fd));
  }
  ~Connection() { close(__conn_fd_); }

  void          create_transaction();
  struct pollfd create_pollfd() const;
  bool          receive_request();
  void          erase_front_transaction() { __transaction_queue_.pop_front(); }
  Transaction  &get_front_transaction() { return __transaction_queue_.front(); }
  const Transaction &get_front_transaction() const {
    return __transaction_queue_.front();
  }
  Transaction &get_back_transaction() { return __transaction_queue_.back(); }
  
};

#endif /* SRCS_EVENT_CONNECTION_HPP */
