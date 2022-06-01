#include "utils/syscall_wrapper.hpp"

#include <dirent.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>

#include "utils/utils.hpp"

// TODO: exitすべきか調査
connFd xaccept(listenFd listen_fd) {
  connFd conn_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
  if (conn_fd == -1) {
    ERROR_LOG_WITH_ERRNO("accept()) failed.");
    exit(EXIT_FAILURE);
  }
  return conn_fd;
}

// TODO: exitすべきか調査
int xpoll(struct pollfd *fds, nfds_t nfds, int timeout) {
  int nready = poll(fds, nfds, timeout);
  if (nready == -1) {
    ERROR_LOG_WITH_ERRNO("poll() failed");
    exit(EXIT_FAILURE);
  }
  return nready;
}

// TODO: fdの上限によるエラーに関してexitしていいか
DIR *xopendir(const char *name) {
  DIR *dir = opendir(name);
  if (dir == NULL) {
    ERROR_LOG_WITH_ERRNO("opendir() failed");
    exit(EXIT_FAILURE);
  }
  return dir;
}

struct dirent *xreaddir(DIR *dir) {
  errno                   = 0;
  struct dirent *dir_read = readdir(dir);
  if (errno != 0) {
    ERROR_LOG_WITH_ERRNO("readdir() failed");
    exit(EXIT_FAILURE);
  }
  return dir_read;
}

void xclosedir(DIR *dir) {
  if (closedir(dir) == -1) {
    ERROR_LOG_WITH_ERRNO("closedir() failed");
    exit(EXIT_FAILURE);
  }
}
