#include "event/Server.hpp"

#include "event/Connection.hpp"
#include "socket/ListenSocket.hpp"
#include "utils/syscall_wrapper.hpp"
#include "utils/utils.hpp"

void Server::run_loop() {
  std::cerr << "start server process" << std::endl;
  for (;;) {
    _socket_map_.close_timedout_socket();
    std::vector<struct pollfd> pollfds = _socket_map_.pollfds();
    int nready = xpoll(&pollfds[0], pollfds.size(), 5000);
    std::vector<struct pollfd>::iterator it = pollfds.begin();
    for (; it != pollfds.end() && 0 < nready; it++) {
      if (it->revents == 0) {
        continue;
      }
      nready--;
      SocketMapOp socket_map_op =
          _socket_map_.handle_socket_event(it->fd, it->revents);
      _socket_map_.do_map_operation(socket_map_op);
    }
  }
}
