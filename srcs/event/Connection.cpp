#include "event/Connection.hpp"

/*
  コネクションが持つリクエストのキューの状態として、、、
  1, リクエストを一つも持たない。
  2, リクエストがあり、キューの最後のリクエストは読み取り済み。
  (状態1, 2はデータを受信したら、状態3へ移行する）
  3, リクエストがあり、キューの最後のリクエストがヘッダーを受信中
  4, リクエストがあり、キューの最後のリクエストがbodyを受信中
 */

void Connection::parse_buffer(const std::string &data) {
  buffer_.append(data);
  while (1) {
    Request &request = request_queue_.back();
    switch (get_last_state()) {
    case RECEIVING_HEADER:
      if (buffer_.find("\r\n\r\n") == std::string::npos) {
        return;
      }
      // bufferをheaderの区切り文字まで切り出して、headerに格納。
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
      // bufferをcontentLength分切り出して、bodyに格納。
      request.state_ = PENDING;
      break;
    default:
      request_queue_.push_back(Request());
      break;
    }
  }
}