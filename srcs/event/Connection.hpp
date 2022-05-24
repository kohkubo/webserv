#ifndef SRCS_EVENT_CONNECTION_HPP
#define SRCS_EVENT_CONNECTION_HPP

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

public:
  Connection() {}
  Connection(confGroup conf_group)
      : __conf_group_(conf_group) {}
  ~Connection() {}

  Transaction &front_transaction() { return __transaction_queue_.front(); }
  const Transaction &front_transaction() const {
    return __transaction_queue_.front();
  }
  void pop_front_queue() { __transaction_queue_.pop_front(); }

  bool is_sending() const;
  void create_transaction(const std::string &data);
  void send_response(connFd conn_fd);
  bool receive_request(connFd conn_fd);
};

#endif /* SRCS_EVENT_CONNECTION_HPP */
