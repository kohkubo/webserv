#ifndef SRCS_UTILS_SYSCALL_WRAPPER_HPP
#define SRCS_UTILS_SYSCALL_WRAPPER_HPP

#include <dirent.h>
#include <poll.h>

typedef int listenFd;
typedef int connFd;

connFd         xaccept(listenFd listen_fd);
int            xpoll(struct pollfd *fds, nfds_t nfds, int timeout);
struct dirent *xreaddir(DIR *dir);
void           xclosedir(DIR *dir);

#endif /* SRCS_UTILS_SYSCALL_WRAPPER_HPP */
