#include "event/Server.hpp"

#include "event/Connection.hpp"
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

void Server::_add_listenfd_to_pollfds(
    const std::map<listenFd, confGroup> &conf_group_map) {
  std::map<listenFd, confGroup>::const_iterator it = conf_group_map.begin();
  for (; it != conf_group_map.end(); it++) {
    struct pollfd new_pfd = {it->first, POLLIN, 5000};
    _pollfds_.push_back(new_pfd);
  }
}

void Server::_add_connfd_to_pollfds(
    const std::map<connFd, Connection> &connection_map) {
  std::map<connFd, Connection>::const_iterator it = connection_map.begin();
  for (; it != connection_map.end(); it++) {
    const Connection &connection = it->second;
    struct pollfd     pfd        = connection.create_pollfd();
    _pollfds_.push_back(pfd);
  }
}

void Server::_connection_receive_handler(Connection &connection) {
  bool is_socket_closed_from_client = connection.append_receive_buffer();
  if (is_socket_closed_from_client) {
    LOG("[LOG] got FIN from connection");
    _terminate_connection(connection);
    return;
  }
  connection.create_sequential_transaction();
}

void Server::_terminate_connection(Connection &connection) {
  connection.close();
  _connection_map_.erase(connection.conn_fd());
}

void Server::_insert_connection_map(listenFd listen_fd) {
  connFd conn_fd = xaccept(listen_fd);
  LOG("[LOG] insert " << conn_fd << " to connection");
  _connection_map_.insert(std::make_pair(
      conn_fd, Connection(conn_fd, _conf_group_map_[listen_fd])));
}

void Server::run_loop() {
  ERROR_LOG("start server process");
  for (;;) {
    _close_timedout_connection(_connection_map_);
    _reset_pollfds();
    // TODO: timeoutは仮 nakamoto
    int nready = xpoll(&_pollfds_[0], _pollfds_.size(), 5000);
    std::vector<struct pollfd>::iterator it = _pollfds_.begin();
    for (; it != _pollfds_.end() && 0 < nready; it++) {
      if (it->revents == 0) {
        continue;
      }
      nready--;
      bool is_listen_socket = static_cast<bool>(_conf_group_map_.count(it->fd));
      if (is_listen_socket) {
        _insert_connection_map(it->fd);
        continue;
      }
      if ((it->revents & (POLLHUP | POLLERR)) != 0) {
        LOG("[LOG] connection (or write end of connection) was closed.");
        _terminate_connection(_connection_map_.find(it->fd)->second);
        continue;
      }
      if ((it->revents & POLLIN) != 0) {
        LOG("[LOG] got POLLIN  event of fd " << it->fd);
        _connection_receive_handler(_connection_map_.find(it->fd)->second);
      }
      if ((it->revents & POLLOUT) != 0) {
        LOG("[LOG] got POLLOUT event of fd " << it->fd);
        _connection_map_.find(it->fd)->second.send_front_response();
      }
    }
  }
}
