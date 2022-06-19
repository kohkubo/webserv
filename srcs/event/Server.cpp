#include "event/Server.hpp"

#include "event/Connection.hpp"
#include "socket/ListenSocket.hpp"
#include "utils/syscall_wrapper.hpp"
#include "utils/utils.hpp"

void Server::_close_timedout_connection(
    std::map<connFd, Connection> &connection_map) {
  std::map<connFd, Connection>::const_iterator it = connection_map.begin();
  while (it != connection_map.end()) {
    if (it->second.is_timed_out()) {
      connFd erase_conn_fd = it->first;
      it->second.close();
      it++;
      connection_map.erase(erase_conn_fd);
    } else {
      it++;
    }
  }
}

void Server::run_loop() {
  std::cerr << "start server process" << std::endl;
  for (;;) {
    _close_timedout_connection(_connection_map_);
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
