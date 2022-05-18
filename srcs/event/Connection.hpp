#ifndef SRCS_EVENT_CONNECTION
#define SRCS_EVENT_CONNECTION

#include "event/Request.hpp"
#include <deque>
#include <string>
#include <vector>

// TODO: string -> vector<char>

typedef class ServerConfig ServerConfig;

class Connection {
private:
  std::deque<Request> __request_queue_;
  std::string         __buffer_;
  conf_group         *__config_;

private:
  std::string __cut_buffer(std::size_t len);

public:
  Connection()
      : __config_(NULL) {}
  Connection(conf_group *config)
      : __config_(config) {}
  ~Connection() {}

  Request &get_last_request() {
    if (__request_queue_.empty())
      __request_queue_.push_back(Request());
    return __request_queue_.back();
  }

  Request &get_front_request() {
    if (__request_queue_.empty())
      __request_queue_.push_back(Request());
    return __request_queue_.front();
  }

  void         erase_front_req() { __request_queue_.pop_front(); }

  RequestState get_last_state() {
    if (__request_queue_.empty())
      return NO_REQUEST;
    return __request_queue_.back().get_state();
  }

  bool is_sending() const {
    if (__request_queue_.empty())
      return false;
    RequestState state = __request_queue_.front().get_state();
    return state == SENDING;
  }

  void parse_buffer(const std::string &data);
  void create_response_iter();
};

typedef int conn_fd;

#endif /* SRCS_EVENT_CONNECTION */
