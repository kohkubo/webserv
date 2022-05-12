#include "event/Connection.hpp"

/*
  コネクションが持つリクエストのキューの状態として、、、
  1, リクエストを一つも持たない。
  2, リクエストがあり、キューの最後のリクエストは読み取り済み。
  (状態1, 2はデータを受信したら、状態3へ移行する）
  3, リクエストがあり、キューの最後のリクエストがヘッダーを受信中
  4, リクエストがあり、キューの最後のリクエストがbodyを受信中
 */

// bufferを指定したlen切り出してstringとして返す関数。
std::string Connection::cut_buffer(std::size_t len) {
  std::string res = buffer_.substr(0, len);
  buffer_         = buffer_.substr(len);
  return res;
}

void Connection::parse_buffer(const std::string &data) {
  std::size_t       pos;
  const std::string header_delim("\r\n\r\n");
  buffer_.append(data);
  while (1) {
    Request &request = request_queue_.back();
    switch (get_last_state()) {
    case RECEIVING_HEADER:
      pos = buffer_.find(header_delim);
      if (pos == std::string::npos) {
        return;
      }
      request.request_header_ = cut_buffer(pos + header_delim.size());
      // headerをパース
      if (request.request_info_.is_expected_body()) {
        request.state_ = RECEIVING_BODY;
      } else {
        request.state_ = PENDING;
      }
      break;
    case RECEIVING_BODY:
      // TODO: chunkedのサイズ判定
      if (buffer_.size() < request.request_info_.content_length_) {
        return;
      }
      request.request_body_ = cut_buffer(request.request_info_.content_length_);
      request.state_        = PENDING;
      break;
    default:
      request_queue_.push_back(Request());
      break;
    }
  }
}