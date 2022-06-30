#include <sys/socket.h>

#include "utils/Result.hpp"
#include "utils/utils.hpp"

namespace socket_base {

Result<std::string> receive(int socket_fd, size_t size) {
  char    buffer[size];
  ssize_t n = recv(socket_fd, buffer, size, MSG_DONTWAIT);
  if (n == -1) {
    ERROR_LOG_WITH_ERRNO("recv() failed.");
    return Ok<std::string>("");
  }
  // fin from client
  if (n == 0) {
    return Error<std::string>();
  }
  return Ok<std::string>(std::string(buffer, n));
}

} // namespace socket_base