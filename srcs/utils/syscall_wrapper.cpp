#include "utils/syscall_wrapper.hpp"

#include <dirent.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>
#include <ctime>

#include "utils/utils.hpp"

// TODO: exitすべきか調査
connFd xaccept(listenFd listen_fd) {
  connFd conn_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
  if (conn_fd == -1) {
    ERROR_EXIT_WITH_ERRNO("accept()) failed.");
  }
  return conn_fd;
}

// TODO: exitすべきか調査
int xpoll(struct pollfd *fds, nfds_t nfds, int timeout) {
  int nready = poll(fds, nfds, timeout);
  if (nready == -1) {
    ERROR_EXIT_WITH_ERRNO("poll() failed");
  }
  return nready;
}

struct dirent *xreaddir(DIR *dir) {
  errno                   = 0;
  struct dirent *dir_read = readdir(dir);
  if (errno != 0) {
    ERROR_EXIT_WITH_ERRNO("readdir() failed");
  }
  return dir_read;
}

void xclosedir(DIR *dir) {
  if (closedir(dir) == -1) {
    ERROR_EXIT_WITH_ERRNO("closedir() failed");
  }
}
