#ifndef SRCS_EVENT_RESPONSEMESSAGE_HPP
#define SRCS_EVENT_RESPONSEMESSAGE_HPP

#include <sys/socket.h>
#include <sys/types.h>

#include <string>

typedef int connFd;

enum ResponseState {
  SENDING,
  CLOSING,
};

class ResponseMessage {
private:
  ResponseState __state_;
  std::string   __response_;
  bool          __is_last_response_;
  ssize_t       __send_count_;

public:
  ResponseMessage(std::string response_message, bool is_close)
      : __state_(SENDING)
      , __response_(response_message)
      , __is_last_response_(is_close)
      , __send_count_(0) {}
  ~ResponseMessage() {}

  ResponseState state() const { return __state_; }
  void          set_state(ResponseState state) { __state_ = state; }
  bool          is_last_response() const { return __is_last_response_; }

  void          send_response(connFd conn_fd) {
    const char *rest_str   = __response_.c_str() + __send_count_;
    size_t      rest_count = __response_.size() - __send_count_;
    // TODO:sendのエラー処理
    __send_count_ += send(conn_fd, rest_str, rest_count, MSG_DONTWAIT);
  }

  bool is_send_all() const {
    return __send_count_ == static_cast<ssize_t>(__response_.size());
  }
};

#endif /* SRCS_EVENT_RESPONSEMESSAGE_HPP */
