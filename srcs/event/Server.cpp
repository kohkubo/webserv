#include "event/Server.hpp"

#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>

#include "event/Connection.hpp"
#include "utils/syscall_wrapper.hpp"
#include "utils/utils.hpp"

void Server::__add_listenfd_to_pollfds() {
  std::map<listenFd, confGroup>::const_iterator it = __conf_group_map_.begin();
  for (; it != __conf_group_map_.end(); it++) {
    struct pollfd new_pfd = {it->first, POLLIN, 0};
    __pollfds_.push_back(new_pfd);
  }
}

void Server::__add_connfd_to_pollfds() {
  std::map<connFd, Connection>::const_iterator it = __connection_map_.begin();
  for (; it != __connection_map_.end(); it++) {
    struct pollfd pfd = {it->first, 0, 0};
    if (it->second.is_sending()) {
      pfd.events = POLLIN | POLLOUT;
    } else if (it->second.is_sending() &&
               it->second.front_transaction().is_close()) {
      pfd.events = POLLOUT;
    } else {
      pfd.events = POLLIN;
    }
    __pollfds_.push_back(pfd);
  }
}

void Server::__connection_receive_handler(connFd conn_fd) {
  const int         buf_size = 2048;
  std::vector<char> buf(buf_size);
  ssize_t           rc = recv(conn_fd, &buf[0], buf_size, MSG_DONTWAIT);
  switch (rc) {
  case -1:
    error_log_with_errno("recv() failed.");
    break;
  case 0:
    close(conn_fd);
    __connection_map_.erase(conn_fd);
    return;
  default:
    std::string recv_data = std::string(buf.begin(), buf.begin() + rc);
    __connection_map_[conn_fd].create_transaction(recv_data);
    break;
  }
}

void Server::__connection_send_handler(connFd conn_fd) {
  Transaction &transaction = __connection_map_[conn_fd].front_transaction();
  transaction.send_response(conn_fd);
  if (transaction.is_send_all()) {
    if (transaction.is_close()) {
      shutdown(conn_fd, SHUT_WR);
      transaction.set_transaction_state(CLOSING);
      return;
    }
    __connection_map_[conn_fd].erase_front_transaction();
  }
}

void Server::__insert_connection_map(connFd conn_fd) {
  __connection_map_.insert(
      std::make_pair(xaccept(conn_fd), Connection(__conf_group_map_[conn_fd])));
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
      int listen_flg = __conf_group_map_.count(it->fd);
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