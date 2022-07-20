#include "event/Server.hpp"

#include <poll.h>

#include <iostream>
#include <vector>

#include "socket/SocketMap.hpp"
#include "socket/SocketMapActions.hpp"
#include "utils/syscall_wrapper.hpp"

void Server::run_loop() {
  std::cerr << "start server process" << std::endl;
  for (;;) {
    _socket_map_.close_timedout_socket();
    std::vector<struct pollfd> pollfds = _socket_map_.create_pollfds();
    int                        nready = poll(&pollfds[0], pollfds.size(), 5000);
    if (nready == -1) {
      ERROR_LOG_WITH_ERRNO("poll() failed");
      continue;
    }
    std::vector<struct pollfd>::iterator it = pollfds.begin();
    for (; it != pollfds.end() && 0 < nready; it++) {
      if (it->revents == 0) {
        continue;
      }
      nready--;
      ns_socket::SocketMapActions socket_map_actions =
          _socket_map_.handle_socket_event(it->fd, it->revents);
      socket_map_actions.do_action(_socket_map_);
    }
  }
}
