#include "event/Server.hpp"

#include <poll.h>

#include <iostream>
#include <vector>

#include "socket/SocketMap.hpp"
#include "socket/SocketMapAction.hpp"
#include "utils/syscall_wrapper.hpp"

void Server::run_loop() {
  std::cerr << "start server process" << std::endl;
  for (;;) {
    _socket_map_.close_timedout_socket();
    std::vector<struct pollfd> pollfds = _socket_map_.create_pollfds();
    int nready = xpoll(&pollfds[0], pollfds.size(), 5000);
    std::vector<struct pollfd>::iterator it = pollfds.begin();
    for (; it != pollfds.end() && 0 < nready; it++) {
      if (it->revents == 0) {
        continue;
      }
      nready--;
      SocketMapAction socket_map_action =
          _socket_map_[it->fd]->handle_event(it->revents);
      _socket_map_.do_socket_map_action(socket_map_action);
    }
  }
}
