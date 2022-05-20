#ifndef SRCS_EVENT_CONNECTION_HPP
#define SRCS_EVENT_CONNECTION_HPP

#include "event/Transaction.hpp"
#include <deque>
#include <string>
#include <vector>

// TODO: string -> vector<char>

typedef class Config Config;

// TODO: こっちでresponse, stateを持つ
class Connection {
private:
  std::deque<Transaction> __transaction_queue_;
  std::string             __buffer_;
  conf_group             *__config_;

private:
  std::string __cut_buffer(std::size_t len);

public:
  Connection()
      : __config_(NULL) {}
  Connection(conf_group *config)
      : __config_(config) {}
  ~Connection() {}

  Transaction &get_last_request() {
    if (__transaction_queue_.empty())
      __transaction_queue_.push_back(Transaction());
    return __transaction_queue_.back();
  }

  Transaction &get_front_request() {
    if (__transaction_queue_.empty())
      __transaction_queue_.push_back(Transaction());
    return __transaction_queue_.front();
  }

  void         erase_front_req() { __transaction_queue_.pop_front(); }

  RequestState get_last_state() {
    if (__transaction_queue_.empty())
      return NO_REQUEST;
    return __transaction_queue_.back().get_state();
  }

  bool is_sending() const {
    if (__transaction_queue_.empty())
      return false;
    RequestState state = __transaction_queue_.front().get_state();
    return state == SENDING;
  }

  void parse_buffer(const std::string &data);
  void create_response_iter();
};

#endif /* SRCS_EVENT_CONNECTION_HPP */
