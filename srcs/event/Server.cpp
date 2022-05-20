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
  std::map<listen_fd, conf_group>::const_iterator it = __listen_fd_map_.begin();
  for (; it != __listen_fd_map_.end(); it++) {
    struct pollfd new_pfd = {it->first, POLLIN, 0};
    __pollfds_.push_back(new_pfd);
  }
}

void Server::__add_connfd_to_pollfds() {
  std::map<int, Connection>::const_iterator it = __conn_fd_map_.begin();
  for (; it != __conn_fd_map_.end(); it++) {
    struct pollfd pfd = {it->first, 0, 0};
    if (it->second.is_sending()) {
      pfd.events = POLLIN | POLLOUT;
    } else {
      pfd.events = POLLIN;
    }
    __pollfds_.push_back(pfd);
  }
}

const int BUF_SIZE = 2048;

// clang-format off
void Server::__connection_receive_handler(connFd conn_fd) {
  // clang-format on
  std::vector<char> buf(BUF_SIZE);
  ssize_t           rc = recv(conn_fd, &buf[0], BUF_SIZE, MSG_DONTWAIT);
  switch (rc) {
  case -1:
    error_log_with_errno("recv() failed.");
    break;
  case 0:
    std::cerr << "closed from client." << std::endl;
    close(conn_fd);
    __conn_fd_map_.erase(conn_fd);
    return;
  default:
    std::string data = std::string(buf.begin(), buf.begin() + rc);
    __conn_fd_map_[conn_fd].parse_buffer(data);
    // cgi用のstateが必要になるかも
    __conn_fd_map_[conn_fd].create_response_iter();
    break;
  }
}

void Server::__connection_send_handler(connFd conn_fd) {
  Transaction &transaction = __conn_fd_map_[conn_fd].get_front_request();
  transaction.send_response(conn_fd);
  if (transaction.is_send_completed()) {
    if (transaction.is_close()) {
      shutdown(conn_fd, SHUT_WR);
      transaction.set_state(CLOSING);
      return;
    }
    __conn_fd_map_[conn_fd].erase_front_req();
  }
}

void Server::__insert_connection_map(connFd conn_fd) {
  __conn_fd_map_.insert(
      std::make_pair(xaccept(conn_fd), Connection(&__listen_fd_map_[conn_fd])));
}

HandlerState Server::__state(std::vector<struct pollfd>::iterator it) {
  if (!it->revents) {
    return NO_REVENTS;
  }
  int listen_flg = __listen_fd_map_.count(it->fd);
  if (listen_flg) {
    return INSERT;
  }
  if (it->revents & POLLIN) {
    return RECV;
  } else if (it->revents & POLLOUT) {
    return SEND;
  }
  return ERROR;
}

void Server::run_loop() {
  while (1) {
    __reset_pollfds();
    int nready = xpoll(&__pollfds_[0], __pollfds_.size(), 0);
    std::vector<struct pollfd>::iterator it = __pollfds_.begin();
    for (; it != __pollfds_.end() && 0 < nready; it++) {
      switch (__state(it)) {
      case NO_REVENTS:
        continue;
      case INSERT:
        __insert_connection_map(it->fd);
        continue;
      case RECV:
        __connection_receive_handler(it->fd);
        nready--;
        continue;
      case SEND:
        __connection_send_handler(it->fd);
        nready--;
        continue;
      default:
        error_log_with_errno("run_loop(): error state.");
        nready--;
        continue;
      }
    }
  }
}