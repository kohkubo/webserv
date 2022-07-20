#ifndef SRCS_UTILS_SYSCALL_WRAPPER_HPP
#define SRCS_UTILS_SYSCALL_WRAPPER_HPP

#include <dirent.h>
#include <poll.h>

typedef int listenFd;
typedef int connFd;

struct dirent *xreaddir(DIR *dir);
void           xclosedir(DIR *dir);

#endif /* SRCS_UTILS_SYSCALL_WRAPPER_HPP */
