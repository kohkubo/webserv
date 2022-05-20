#include "utils/syscall_wrapper.hpp"

#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>

#include "utils/utils.hpp"

// TODO: exitすべきか調査
int xaccept(listenFd listen_fd) {
  int connection_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
  if (connection_fd == -1) {
    error_log_with_errno("accept()) failed.");
    exit(EXIT_FAILURE);
  }
  std::cout << "listen fd: " << listen_fd << std::endl;
  std::cout << "connection fd: " << connection_fd << std::endl;
  return connection_fd;
}

// TODO: exitすべきか調査
int xpoll(struct pollfd *fds, nfds_t nfds, int timeout) {
  int nready = poll(fds, nfds, timeout);
  if (nready == -1) {
    error_log_with_errno("poll() failed");
    exit(EXIT_FAILURE);
  }
  return nready;
}
