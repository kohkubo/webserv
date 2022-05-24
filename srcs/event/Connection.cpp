#include "event/Connection.hpp"

#include "config/Config.hpp"
#include "http/const/const_delimiter.hpp"

Transaction &Connection::__get_last_request() {
  if (__transaction_queue_.empty())
    __transaction_queue_.push_back(Transaction());
  return __transaction_queue_.back();
}

bool Connection::is_sending() const {
  if (__transaction_queue_.empty())
    return false;
  TransactionState transaction_state =
      __transaction_queue_.front().get_transaction_state();
  return transaction_state == SENDING;
}

void Connection::create_transaction(const std::string &data) {
  __buffer_.append(data);
  while (1) {
    Transaction     &transaction = __get_last_request();
    TransactionState updated_state =
        transaction.handle_state(__buffer_, __conf_group_);
    if (updated_state != SENDING) {
      return;
    }
    __transaction_queue_.push_back(Transaction());
    continue;
  }
}
