#ifndef SRCS_EVENT_RESPONSE_HPP
#define SRCS_EVENT_RESPONSE_HPP

#include <sys/socket.h>
#include <sys/types.h>

#include <string>

typedef int connFd;

class Response {
public:
  enum FdState {
    SENDING,  // おくってないか、送信中
    CLOSING,  // クライアントからのclose待ち
    COMPLETE, // 送信終了
    WAITING,  // CGI,FILE待ち
  };

private:
  FdState     _state_;
  std::string _response_;
  bool        _is_last_response_;
  ssize_t     _send_count_;

private:
  inline bool _is_send_all() const {
    return _send_count_ == static_cast<ssize_t>(_response_message_.size());
  }

public:
  Response(std::string response_message, bool is_close)
      : _state_(SENDING)
      , _response_message_(response_message)
      , _is_last_response_(is_close)
      , _send_count_(0) {}
  ~Response() {}

  bool    is_sending() const { return _state_ == SENDING; }
  FdState send(connFd conn_fd) {
    const char *rest_str   = _response_.c_str() + _send_count_;
    size_t      rest_count = _response_.size() - _send_count_;
    _send_count_ += ::send(conn_fd, rest_str, rest_count, MSG_DONTWAIT);
    if (_is_last_response_ && _is_send_all()) {
      shutdown(conn_fd, SHUT_WR);
      _state_ = CLOSING;
    } else if (_is_send_all()) {
      _state_ = COMPLETE;
    }
    return _state_;
  }
};

#endif /* SRCS_EVENT_RESPONSE_HPP */
