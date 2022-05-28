#include "utils/syscall_wrapper.hpp"

#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>

#include "utils/utils.hpp"

// TODO: exitすべきか調査
connFd xaccept(listenFd listen_fd) {
  connFd conn_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
  if (conn_fd == -1) {
    error_log_with_errno("accept()) failed.");
    exit(EXIT_FAILURE);
  }
  return conn_fd;
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

// TODO: fdの上限によるエラーに関してexitしていいか
DIR *xopendir(const char *name) {
  DIR *dir = opendir(name);
  if (dir == NULL) {
    error_log_with_errno("opendir() failed");
    exit(EXIT_FAILURE);
  }
  return dir;
}
