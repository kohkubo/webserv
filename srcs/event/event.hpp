#ifndef SRCS_EVENT_EVENT_HPP
#define SRCS_EVENT_EVENT_HPP

#include "config/ServerConfig.hpp"
#include <map>
#include <sys/types.h>
#include <poll.h>

int  open_new_socket(const ServerConfig &config);
void listen_event(const server_group_type &server_group);

template <typename T>
int set_fd_list(fd_set *readfds, const std::map<int, T> &list) {
  int                                       max_fd = -1;
  typename std::map<int, T>::const_iterator it     = list.begin();
  for (; it != list.end(); it++) {
    FD_SET(it->first, readfds);
    max_fd = std::max(it->first, max_fd);
  }
  return max_fd;
}

template <typename T>
void set_fd_list(struct pollfd *pfds, int start_idx, const std::map<int, T> &list) {
  typename std::map<int, T>::const_iterator it     = list.begin();
  for (; it != list.end(); it++) {
    pfds[start_idx].fd = it->first;
    pfds[start_idx].events = POLLIN;
    start_idx++;
  }
}

typedef std::map<int, int> connection_list_type; // <accetpted fd, listen fd>
typedef connection_list_type::iterator connection_list_iterator;

#endif /* SRCS_EVENT_EVENT_HPP */
