#include "event.hpp"

// socket_fd + connection_fdをreadfdsに加える。
fd_set create_readfds(const socket_list_type &socket_list, int &nfds) {
  int    res;
  fd_set readfds;

  nfds = -1;
  FD_ZERO(&readfds);
  res  = set_fd_list(&readfds, socket_list);
  nfds = std::max(nfds, res);
  return readfds;
}
