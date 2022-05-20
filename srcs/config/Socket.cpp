#include "config/Socket.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "config/Config.hpp"
#include "utils/utils.hpp"

void Socket::__set_listen_fd() {
  __listen_fd_ =
      socket(__info_->ai_family, __info_->ai_socktype, __info_->ai_protocol);
  if (__listen_fd_ == -1) {
    error_log_with_errno("socket() failed.");
    exit(EXIT_FAILURE);
  }
  if (fcntl(__listen_fd_, F_SETFL, O_NONBLOCK) == -1) {
    error_log_with_errno("fcntl() failed.");
    exit(EXIT_FAILURE);
  }
  int on = 1;
  if (setsockopt(__listen_fd_, SOL_SOCKET, SO_REUSEADDR, (const void *)&on,
                 sizeof(on)) == -1) {
    error_log_with_errno("setsockopt() failed.");
    close(__listen_fd_);
    exit(EXIT_FAILURE);
  }
}

void Socket::__set_listen_passive_socket() {
  if (listen(__listen_fd_, SOMAXCONN) == -1) {
    error_log_with_errno("listen() failed.");
    close(__listen_fd_);
    exit(EXIT_FAILURE);
  }
}

void Socket::__set_bind_socket() {
  if (bind(__listen_fd_, __info_->ai_addr, __info_->ai_addrlen) == -1) {
    error_log_with_errno("bind() failed.");
    close(__listen_fd_);
    exit(EXIT_FAILURE);
  }
}
