#ifndef SRCS_EVENT_TIMEOUT_HPP
#define SRCS_EVENT_TIMEOUT_HPP

#include <cstdlib>

#include "utils/syscall_wrapper.hpp"

class Timeout {
private:
  std::time_t __last_event_time_;

public:
  Timeout() { update_last_time_event(); }

  void update_last_time_event() { __last_event_time_ = xtime(); }

  bool is_timeout_over() const {
    std::time_t now = xtime();
    return (now - __last_event_time_) >= 60;
  }
};

#endif /* SRCS_EVENT_TIMEOUT_HPP */
