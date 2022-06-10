#ifndef SRCS_EVENT_RESPONSE_HPP
#define SRCS_EVENT_RESPONSE_HPP

#include <sys/socket.h>
#include <sys/types.h>

#include <string>

typedef int connFd;

enum ResponseState {
  SENDING,
  CLOSING,
  COMPLETE,
};

class Response {
private:
  ResponseState __state_;
  std::string   __response_;
  bool          __is_last_response_;
  ssize_t       __send_count_;

private:
  bool __is_send_all() const {
    return __send_count_ == static_cast<ssize_t>(__response_.size());
  }

public:
  Response(std::string response_message, bool is_close)
      : __state_(SENDING)
      , __response_(response_message)
      , __is_last_response_(is_close)
      , __send_count_(0) {}
  ~Response() {}

  ResponseState state() const { return __state_; }
  ResponseState send(connFd conn_fd) {
    const char *rest_str   = __response_.c_str() + __send_count_;
    size_t      rest_count = __response_.size() - __send_count_;
    __send_count_ += ::send(conn_fd, rest_str, rest_count, MSG_DONTWAIT);
    if (__is_send_all() && !__is_last_response_) {
      return COMPLETE;
    }
    if (__is_last_response_) {
      shutdown(conn_fd, SHUT_WR);
      return CLOSING;
    }
    return SENDING;
  }
};

#endif /* SRCS_EVENT_RESPONSE_HPP */
