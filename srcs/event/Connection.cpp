#include "event/Connection.hpp"

#include "config/Config.hpp"
#include "http/const/const_delimiter.hpp"

// bufferを指定したlen切り出してstringとして返す関数。
std::string Connection::__cut_buffer(std::size_t len) {
  std::string res = __buffer_.substr(0, len);
  __buffer_       = __buffer_.substr(len);
  return res;
}

void Connection::create_transaction(const std::string &data) {
  std::size_t pos;
  tokenVector header_tokens;

  __buffer_.append(data);
  while (1) {
    Transaction &transaction = get_last_request();
    switch (get_last_state()) {
    case RECEIVING_HEADER:
      pos = __buffer_.find(HEADER_SP);
      if (pos == std::string::npos) {
        return;
      }
      transaction.parse_header(__cut_buffer(pos + HEADER_SP.size()));
      transaction.detect_config(__conf_group_);
      break;
    case RECEIVING_BODY:
      // TODO: chunkedのサイズ判定
      if (__buffer_.size() < transaction.get_body_size()) {
        return;
      }
      transaction.parse_body(__cut_buffer(transaction.get_body_size()));
      break;
    default:
      __transaction_queue_.push_back(Transaction());
      break;
    }
  }
}

// キューの中にあるresponse生成待ちのrequestのresponseを生成する。
// TODO: responseは複数存在しないので、できた段階で送るように変更する 2022/05/22
// 16:50 nakamoto okhkubo話し合い
void Connection::create_response_iter() {
  std::deque<Transaction>::iterator it = __transaction_queue_.begin();
  for (; it != __transaction_queue_.end(); it++) {
    (*it).create_response();
  }
}
