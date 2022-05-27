#ifndef SRCS_EVENT_CONNECTION_HPP
#define SRCS_EVENT_CONNECTION_HPP

#include <poll.h>

#include <deque>
#include <string>
#include <vector>

#include "event/Transaction.hpp"

// TODO: string -> vector<char>

typedef class Config Config;
typedef int          connFd;

class Connection {
private:
  std::deque<Transaction> __transaction_queue_;
  std::string             __buffer_;
  confGroup               __conf_group_;

private:
  Transaction &__get_last_transaction();
  void         __create_transaction(const std::string &data);

public:
  Connection() {}
  Connection(confGroup conf_group)
      : __conf_group_(conf_group) {}
  ~Connection() {}

  struct pollfd create_pollfd(connFd conn_fd) const;
  bool          receive_request(connFd conn_fd);
  void          erase_front_transaction() { __transaction_queue_.pop_front(); }
  Transaction  &get_front_transaction() { return __transaction_queue_.front(); }
  const Transaction &get_front_transaction() const {
    return __transaction_queue_.front();
  }
};

#endif /* SRCS_EVENT_CONNECTION_HPP */
