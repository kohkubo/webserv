#ifndef SRCS_SOCKET_TIMEOUT_HPP
#define SRCS_SOCKET_TIMEOUT_HPP

#include <ctime>

namespace socket_base {

class Timeout {
private:
  std::time_t _timeout_seconds_;
  std::time_t _last_event_time_;

private:
  static std::time_t _time_now() { return std::time(NULL); }

public:
  Timeout()
      : _timeout_seconds_(-1)
      , _last_event_time_(_time_now()) {}
  Timeout(const std::time_t timeout_seconds)
      : _timeout_seconds_(timeout_seconds)
      , _last_event_time_(_time_now()) {}
  ~Timeout() {}
  void update_last_event() { _last_event_time_ = _time_now(); }
  bool is_timed_out() const {
    return std::difftime(_time_now(), _last_event_time_) >= _timeout_seconds_;
  }
};

} // namespace socket_base

#endif /* SRCS_SOCKET_TIMEOUT_HPP */
