#ifndef INCLUDES_EVENT_HPP
#define INCLUDES_EVENT_HPP

#include <map>

#include "Socket.hpp"

typedef std::map<int, std::vector<ServerConfig> > socket_list_type;

void listen_event(const std::vector<ServerConfig> &server_list);
socket_list_type
     create_socket_map(const std::vector<ServerConfig> &server_list);
bool is_same_socket(const ServerConfig &serv_x, const ServerConfig &serv_y);
std::pair<fd_set, int> create_readfds(const socket_list_type &socket_list);
void                   close_all_socket(const socket_list_type &socket_list);

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

#endif /* INCLUDES_EVENT_HPP */
