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
void Server::__connection_receive_handler(int conn_fd) {
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

void Server::__connection_send_handler(int conn_fd) {
  Request &request = __conn_fd_map_[conn_fd].get_front_request();
  request.send_response(conn_fd);
  if (request.is_send_completed()) {
    if (request.is_close()) {
      shutdown(conn_fd, SHUT_WR);
      request.set_state(CLOSING);
      return;
    }
    __conn_fd_map_[conn_fd].erase_front_req();
  }
}

void Server::__insert_connection_map(int conn_fd) {
  conn_fd = xaccept(conn_fd);
  __conn_fd_map_.insert(
      std::make_pair(conn_fd, Connection(&__listen_fd_map_[conn_fd])));
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
      int listen_flg = __listen_fd_map_.count(it->fd);
      if (listen_flg) {
        __insert_connection_map(it->fd);
        continue;
      }
      if (it->revents & POLLIN) {
        __connection_receive_handler(it->fd);
      } else if (it->revents & POLLOUT) {
        __connection_send_handler(it->fd);
      }
      // TODO: 他reventsに対する処理
      nready--;
    }
  }
}