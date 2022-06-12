#include "event/Server.hpp"

#include "event/Connection.hpp"
#include "utils/syscall_wrapper.hpp"
#include "utils/utils.hpp"

void Server::__close_timedout_connection(
    std::map<connFd, Connection> &connection_map) {
  std::map<connFd, Connection>::const_iterator it = connection_map.begin();
  while (it != connection_map.end()) {
    if (it->second.is_timed_out()) {
      connFd conn_fd = it->first;
      it++;
      close(conn_fd);
      connection_map.erase(conn_fd);
    } else {
      it++;
    }
  }
}

void Server::__add_listenfd_to_pollfds(
    const std::map<listenFd, confGroup> &conf_group_map) {
  std::map<listenFd, confGroup>::const_iterator it = conf_group_map.begin();
  for (; it != conf_group_map.end(); it++) {
    struct pollfd new_pfd = {it->first, POLLIN, 0};
    __pollfds_.push_back(new_pfd);
  }
}

void Server::__add_connfd_to_pollfds(
    const std::map<connFd, Connection> &connection_map) {
  std::map<connFd, Connection>::const_iterator it = connection_map.begin();
  for (; it != connection_map.end(); it++) {
    const Connection &connection = it->second;
    struct pollfd     pfd        = connection.create_pollfd();
    __pollfds_.push_back(pfd);
  }
}

void Server::__connection_receive_handler(connFd conn_fd) {
  // findされないケースはない
  std::map<connFd, Connection>::iterator it = __connection_map_.find(conn_fd);
  bool is_socket_closed_from_client = it->second.append_receive_buffer();
  if (is_socket_closed_from_client) {
    close(conn_fd);
    __connection_map_.erase(conn_fd);
    return;
  }
  it->second.create_sequential_transaction();
}

void Server::__insert_connection_map(listenFd listen_fd) {
  connFd conn_fd = xaccept(listen_fd);
  __connection_map_.insert(std::make_pair(
      conn_fd, Connection(conn_fd, __conf_group_map_[listen_fd])));
}

void Server::run_loop() {
  ERROR_LOG("start server process");
  while (true) {
    __close_timedout_connection(__connection_map_);
    __reset_pollfds();
    // timeoutは仮
    int nready = xpoll(&__pollfds_[0], __pollfds_.size(), 50000);
    std::vector<struct pollfd>::iterator it = __pollfds_.begin();
    for (; it != __pollfds_.end() && 0 < nready; it++) {
      if (it->revents == 0) {
        continue;
      }
      nready--;
      bool is_listen_socket =
          static_cast<bool>(__conf_group_map_.count(it->fd));
      if (is_listen_socket) {
        __insert_connection_map(it->fd);
        continue;
      }
      if ((it->revents & POLLIN) != 0) {
        __connection_receive_handler(it->fd);
      }
      if ((it->revents & POLLOUT) != 0) {
        // TODO: []からの書き換え、findできないケースある??
        __connection_map_.find(it->fd)->second.send_front_response();
      }
    }
  }
}
