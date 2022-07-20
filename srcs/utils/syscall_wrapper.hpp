#ifndef SRCS_UTILS_SYSCALL_WRAPPER_HPP
#define SRCS_UTILS_SYSCALL_WRAPPER_HPP

#include <dirent.h>
#include <poll.h>

typedef int listenFd;
typedef int connFd;

struct dirent *w_readdir(DIR *dir);
void           w_closedir(DIR *dir);

#endif /* SRCS_UTILS_SYSCALL_WRAPPER_HPP */
