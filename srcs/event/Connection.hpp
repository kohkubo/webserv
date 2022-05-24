#ifndef SRCS_EVENT_CONNECTION_HPP
#define SRCS_EVENT_CONNECTION_HPP

#include <deque>
#include <string>
#include <vector>

#include "event/Transaction.hpp"

// TODO: string -> vector<char>

typedef class Config Config;

class Connection {
private:
  std::deque<Transaction> __transaction_queue_;
  std::string             __buffer_;
  confGroup               __conf_group_;

private:
  Transaction &__get_last_request();

public:
  Connection() {}
  Connection(confGroup conf_group)
      : __conf_group_(conf_group) {}
  ~Connection() {}

  Transaction &get_front_request() { return __transaction_queue_.front(); }
  void         erase_front_req() { __transaction_queue_.pop_front(); }

  bool         is_sending() const;
  void         create_transaction(const std::string &data);
};

#endif /* SRCS_EVENT_CONNECTION_HPP */
