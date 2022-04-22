#ifndef INCLUDES_EVENT_HPP
#define INCLUDES_EVENT_HPP

#include <map>

#include "Socket.hpp"

void listen_event();

std::map<int, Socket>
create_socket_map(const std::vector<ServerConfig> &server_list);
std::pair<fd_set, int>
     create_readfds(const std::map<int, Socket> &socket_list,
                    const std::map<int, int>    &connection_list);

bool add_connection_list(int socket_fd, std::map<int, int> &connection_list);

template <typename T>
int set_fd_list(fd_set *readfds, const std::map<int, T> &list) {
  int                                 maxfd = -1;
  typename std::map<int, T>::iterator it    = list.begin();
  for (; it != list.end(); it++) {
    FD_SET(it->first, readfds);
    if (it->first > maxfd)
      maxfd = it->first;
  }
  return maxfd;
}

#endif /* INCLUDES_EVENT_HPP */
