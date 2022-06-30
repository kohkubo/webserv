#include <sys/socket.h>

#include "socket.hpp"
#include "utils/utils.hpp"

namespace ns_socket {

ReceiveResult receive(int socket_fd, size_t size) {
  char    buffer[size];
  ssize_t n = recv(socket_fd, buffer, size, MSG_DONTWAIT);
  if (n == -1) {
    ERROR_LOG_WITH_ERRNO("recv() failed.");
    return ReceiveResult(n, "");
  }
  // fin from client
  if (n == 0) {
    return ReceiveResult(n, "");
  }
  return ReceiveResult(n, std::string(buffer, n));
}

} // namespace ns_socket