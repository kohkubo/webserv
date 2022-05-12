#include "event/Connection.hpp"
#include "config/ServerConfig.hpp"
#include "http/const/const_delimiter.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/request/request_parse.hpp"
#include "http/response/response.hpp"
#include "utils/tokenize.hpp"

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
  std::size_t  pos;
  token_vector header_tokens;

  buffer_.append(data);
  while (1) {
    Request &request = request_queue_.back();
    switch (get_last_state()) {
    case RECEIVING_HEADER:
      pos = buffer_.find(HEADER_SP);
      if (pos == std::string::npos) {
        return;
      }
      request.request_header_ = cut_buffer(pos + HEADER_SP.size());
      parse_request_header(request.request_info_, request.request_header_);
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
      parse_request_body(request.request_info_, request.request_body_);
      request.state_ = PENDING;
      break;
    default:
      request_queue_.push_back(Request());
      break;
    }
  }
}

// キューの中にあるresponse生成待ちのrequestのresponseを生成する。
void Connection::make_response(
    const std::vector<const ServerConfig *> &server_list) {
  (void)server_list;
  ServerConfig                  proper_conf;
  std::deque<Request>::iterator it = request_queue_.begin();
  for (; it != request_queue_.end(); it++) {
    if ((*it).state_ == PENDING) {
      // TODO: リクエストに対して正しいserverconfを選択する。
      http_message_map response_info =
          create_response_info(proper_conf, (*it).request_info_);
      (*it).response_ = make_message_string(response_info);
      (*it).state_    = SENDING;
    }
  }
}
