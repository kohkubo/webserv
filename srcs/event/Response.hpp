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
  inline bool __is_send_all() const {
    return __send_count_ == static_cast<ssize_t>(__response_.size());
  }

public:
  Response(std::string response_message, bool is_close)
      : __state_(SENDING)
      , __response_(response_message)
      , __is_last_response_(is_close)
      , __send_count_(0) {}
  ~Response() {}

  bool          is_sending() const { return __state_ == SENDING; }
  ResponseState send(connFd conn_fd) {
    const char *rest_str   = __response_.c_str() + __send_count_;
    size_t      rest_count = __response_.size() - __send_count_;
    std::cout << __response_ << std::endl;
    __send_count_ += ::send(conn_fd, rest_str, rest_count, MSG_DONTWAIT);
    if (__is_last_response_ && __is_send_all()) {
      shutdown(conn_fd, SHUT_WR);
      __state_ = CLOSING;
    } else if (__is_send_all()) {
      __state_ = COMPLETE;
    }
    return __state_;
  }
};

#endif /* SRCS_EVENT_RESPONSE_HPP */
