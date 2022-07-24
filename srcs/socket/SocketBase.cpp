#include "socket/SocketBase.hpp"

namespace ns_socket {

ReceiveResult SocketBase::receive_content(size_t buffer_size) const {
  char    buffer[buffer_size];
  ssize_t n = read(_socket_fd_, buffer, buffer_size);
  if (n == -1) {
    ERROR_LOG_WITH_ERRNO("receive_content failed.");
    return ReceiveResult(ReceiveResult::ERROR, "");
  }
  // fin from client
  if (n == 0) {
    return ReceiveResult(ReceiveResult::END, "");
  }
  return ReceiveResult(ReceiveResult::SUCCESS, std::string(buffer, n));
}

} // namespace ns_socket
