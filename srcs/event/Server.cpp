#include "event/Server.hpp"

#include <unistd.h>

#include "event/Connection.hpp"
#include "utils/syscall_wrapper.hpp"

void Server::__add_listenfd_to_pollfds() {
  std::map<listenFd, confGroup>::const_iterator it = __listen_fd_map_.begin();
  for (; it != __listen_fd_map_.end(); it++) {
    struct pollfd new_pfd = {it->first, POLLIN, 0};
    __pollfds_.push_back(new_pfd);
  }
}

void Server::__add_connfd_to_pollfds() {
  std::map<connFd, Connection>::const_iterator it = __conn_fd_map_.begin();
  for (; it != __conn_fd_map_.end(); it++) {
    struct pollfd pfd = it->second.pollfd(it->first);
    __pollfds_.push_back(pfd);
  }
}

void Server::__connection_receive_handler(connFd conn_fd) {
  bool is_close = __conn_fd_map_[conn_fd].receive_request(conn_fd);
  if (is_close) {
    close(conn_fd);
    __conn_fd_map_.erase(conn_fd);
  }
}

void Server::__connection_send_handler(connFd conn_fd) {
  __conn_fd_map_[conn_fd].send_response(conn_fd);
}

void Server::__insert_connection_map(connFd conn_fd) {
  __conn_fd_map_.insert(
      std::make_pair(xaccept(conn_fd), Connection(__listen_fd_map_[conn_fd])));
}

void Server::run_loop() {
  while (1) {
    __reset_pollfds();
    int nready = xpoll(&__pollfds_[0], __pollfds_.size(), 0);
    std::vector<struct pollfd>::iterator it = __pollfds_.begin();
    for (; it != __pollfds_.end() && 0 < nready; it++) {
      if (!it->revents) {
        continue;
      }
      nready--;
      int listen_flg = __listen_fd_map_.count(it->fd);
      if (listen_flg) {
        __insert_connection_map(it->fd);
        continue;
      }
      if (it->revents & POLLIN) {
        __connection_receive_handler(it->fd);
      }
      if (it->revents & POLLOUT) {
        __connection_send_handler(it->fd);
      }
    }
  }
}