#include "event/Response.hpp"

#include <sys/socket.h>
#include <sys/types.h>

void Response::send_message(int socket_fd) {
  const char *rest_str   = __message_.c_str() + __send_count_;
  size_t      rest_count = __message_.size() - __send_count_;
  ssize_t     sc         = send(socket_fd, rest_str, rest_count, MSG_DONTWAIT);
  __send_count_ += sc;
}
