#include "event/Connection.hpp"

#include "config/Config.hpp"
#include "http/const/const_delimiter.hpp"

Transaction &Connection::__get_last_transaction() {
  if (__transaction_queue_.empty())
    __transaction_queue_.push_back(Transaction());
  return __transaction_queue_.back();
}

bool Connection::is_sending() const {
  if (__transaction_queue_.empty())
    return false;
  return front_transaction().is_sending();
}

void Connection::create_transaction(const std::string &data) {
  __buffer_.append(data);
  while (1) {
    Transaction &transaction = __get_last_transaction();
    bool is_continue = transaction.handle_state(__buffer_, __conf_group_);
    if (is_continue) {
      __transaction_queue_.push_back(Transaction());
      continue;
    } else {
      return;
    }
  }
}
