#ifndef SRCS_UTILS_SYSCALL_WRAPPER_HPP
#define SRCS_UTILS_SYSCALL_WRAPPER_HPP

#include <dirent.h>
#include <poll.h>

#include <ctime>

typedef int    listenFd;
typedef int    connFd;

connFd         xaccept(listenFd listen_fd);
int            xpoll(struct pollfd *fds, nfds_t nfds, int timeout);
DIR           *xopendir(const char *name);
struct dirent *xreaddir(DIR *dir);
void           xclosedir(DIR *dir);
std::time_t    xtime();

#endif /* SRCS_UTILS_SYSCALL_WRAPPER_HPP */
