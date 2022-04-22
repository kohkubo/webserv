#ifndef INCLUDES_EVENT_HPP
#define INCLUDES_EVENT_HPP

#include <map>

#include "Socket.hpp"

typedef int socket_fd;

void        listen_event();

std::map<socket_fd, std::vector<ServerConfig> >
create_socket_map(const std::vector<ServerConfig> &server_list);
std::pair<fd_set, int> create_readfds(
    const std::map<socket_fd, std::vector<ServerConfig> > &socket_list);

template <typename T>
int set_fd_list(fd_set *readfds, const std::map<int, T> &list) {
  int                                       maxfd = -1;
  typename std::map<int, T>::const_iterator it    = list.begin();
  for (; it != list.end(); it++) {
    FD_SET(it->first, readfds);
    if (it->first > maxfd)
      maxfd = it->first;
  }
  return maxfd;
}

#endif /* INCLUDES_EVENT_HPP */
