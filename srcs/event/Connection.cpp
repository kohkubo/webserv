#include "event/Connection.hpp"
#include "config/ServerConfig.hpp"
#include "http/const/const_delimiter.hpp"

// bufferを指定したlen切り出してstringとして返す関数。
std::string Connection::__cut_buffer(std::size_t len) {
  std::string res = __buffer_.substr(0, len);
  __buffer_       = __buffer_.substr(len);
  return res;
}

void Connection::parse_buffer(const std::string &data) {
  std::size_t  pos;
  token_vector header_tokens;

  __buffer_.append(data);
  while (1) {
    Request &request = get_last_request();
    switch (get_last_state()) {
    case RECEIVING_HEADER:
      pos = __buffer_.find(HEADER_SP);
      if (pos == std::string::npos) {
        return;
      }
      request.parse_header(__cut_buffer(pos + HEADER_SP.size()));
      break;
    case RECEIVING_BODY:
      // TODO: chunkedのサイズ判定
      if (__buffer_.size() < request.get_body_size()) {
        return;
      }
      request.parse_body(__cut_buffer(request.get_body_size()));
      break;
    default:
      __request_queue_.push_back(Request());
      break;
    }
  }
}

// キューの中にあるresponse生成待ちのrequestのresponseを生成する。
void Connection::create_response_iter() {
  (void)__config_;
  ServerConfig proper_conf;
  while (!__request_queue_.empty()) {
    Request &top_req = __request_queue_.front();
    if (top_req.get_state() != PENDING)
      break;
    // TODO: リクエストに対して正しいserverconfを選択する。
    std::string response_str = top_req.create_response(proper_conf);
    __response_queue_.push_back(Response(response_str, top_req.is_close()));
    __request_queue_.pop_front();
  }
}
