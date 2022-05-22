#ifndef SRCS_EVENT_CONNECTION_HPP
#define SRCS_EVENT_CONNECTION_HPP

#include <deque>
#include <string>
#include <vector>

#include "event/Transaction.hpp"

// TODO: string -> vector<char>

typedef class Config Config;

// TODO: こっちでresponse, stateを持つ
class Connection {
private:
  std::deque<Transaction> __transaction_queue_;
  std::string             __buffer_;
  confGroup               __conf_group_;

private:
  std::string __cut_buffer(std::size_t len);

public:
  Connection() {}
  Connection(confGroup conf_group)
      : __conf_group_(conf_group) {}
  ~Connection() {}

  Transaction &get_last_request() {
    if (__transaction_queue_.empty())
      __transaction_queue_.push_back(Transaction());
    return __transaction_queue_.back();
  }

  // TODO: 分岐不要の話はどうなった?
  Transaction &get_front_request() {
    if (__transaction_queue_.empty())
      __transaction_queue_.push_back(Transaction());
    return __transaction_queue_.front();
  }

  void             erase_front_req() { __transaction_queue_.pop_front(); }

  TransactionState get_last_state() {
    if (__transaction_queue_.empty())
      return NO_REQUEST;
    return __transaction_queue_.back().get_tranction_state();
  }

  bool is_sending() const {
    if (__transaction_queue_.empty())
      return false;
    TransactionState transaction_state =
        __transaction_queue_.front().get_tranction_state();
    return transaction_state == SENDING;
  }

  void create_transaction(const std::string &data);
  void create_response_iter();
};

#endif /* SRCS_EVENT_CONNECTION_HPP */
