#ifndef SRCS_EVENT_TIMEOUT_HPP
#define SRCS_EVENT_TIMEOUT_HPP

#include <ctime>

class Timeout {
private:
  std::time_t __last_event_time_;

public:
  Timeout();
  void update_last_time_event();
  bool is_timeout_over();
};

#endif /* SRCS_EVENT_TIMEOUT_HPP */
