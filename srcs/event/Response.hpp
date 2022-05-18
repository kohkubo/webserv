#ifndef SRCS_EVENT_RESPONSE_HPP
#define SRCS_EVENT_RESPONSE_HPP

#include <string>
#include <sys/types.h>

enum ResponseState {
  SENDING, // レスポンスの送信中。
  CLOSING,
};

class Response {
private:
  ResponseState __state_;
  std::string   __message_;
  ssize_t       __send_count_;
  bool          __is_close_;

public:
  Response(std::string response_message, bool is_close)
      : __state_(SENDING)
      , __message_(response_message)
      , __send_count_(0)
      , __is_close_(is_close) {}

  ResponseState get_state() const { return __state_; }
  void          set_closing() { __state_ = CLOSING; }
  bool          is_close() { return __is_close_; }
  void          send_message(int socket_fd);
  bool          is_send_completed() {
    return __send_count_ == static_cast<ssize_t>(__message_.size());
  }
};

#endif /* SRCS_EVENT_RESPONSE_HPP */
