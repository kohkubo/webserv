#include "event.hpp"

// socket_fd + connection_fdをreadfdsに加える。
std::pair<fd_set, int> create_readfds(const socket_list_type &socket_list) {
  int    nfds = -1, res;
  fd_set readfds;

  FD_ZERO(&readfds);
  res  = set_fd_list(&readfds, socket_list);
  nfds = std::max(nfds, res);
  return std::make_pair(readfds, nfds);
}
