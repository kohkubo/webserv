#ifndef SRCS_SOCKET_SOCKET_HPP
#define SRCS_SOCKET_SOCKET_HPP

#include <string>

#include "utils/Result.hpp"

namespace socket_base {

struct ReceiveResult {
  ssize_t     rc_;
  std::string str_;

  ReceiveResult(ssize_t rc, std::string str)
      : rc_(rc)
      , str_(str) {}
};

ReceiveResult receive(int socket_fd, std::size_t size);

} // namespace socket_base
#endif /* SRCS_SOCKET_SOCKET_HPP */
