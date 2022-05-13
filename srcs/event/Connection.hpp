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

public:
  int socket_fd_;

  Connection()
      : socket_fd_(-1) {}
  Connection(int fd)
      : socket_fd_(fd) {}
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
    return __request_queue_.back().state_;
  }

  bool is_waiting_send() const {
    if (__request_queue_.empty())
      return false;
    RequestState state = __request_queue_.front().state_;
    return state == SENDING;
  }

  void        parse_buffer(const std::string &data);
  std::string cut_buffer(std::size_t len);
  void make_response(const std::vector<const ServerConfig *> &server_list);
};

typedef std::map<int, Connection> connection_list_type;

#endif /* SRCS_EVENT_CONNECTION */
