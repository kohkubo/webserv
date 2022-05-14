#include "event/ConnectionMap.hpp"
#include "utils/syscall_wrapper.hpp"
#include "utils/utils.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void ConnectionMap::add_new_connection(int               listen_fd,
                                       socket_list_type &socket_list) {
  int connection_fd = xaccept(listen_fd);
  __container_.insert(
      std::make_pair(connection_fd, Connection(&socket_list[listen_fd])));
}

void ConnectionMap::receive(int connection_fd) {
  const int         buf_size   = 2048;
  Connection       &connection = __container_[connection_fd];
  std::vector<char> buf(buf_size);
  ssize_t           rc = recv(connection_fd, &buf[0], buf_size, MSG_DONTWAIT);
  if (rc == -1) {
    error_log_with_errno("recv() failed.");
  }
  if (rc == 0) {
    std::cerr << "closed from client." << std::endl;
    close(connection_fd);
    __container_.erase(connection_fd);
    return;
  }
  std::string data = std::string(buf.begin(), buf.begin() + rc);
  connection.parse_buffer(data);
  // cgi用のstateが必要になるかも
  connection.create_response_iter();
}

void ConnectionMap::send(int connection_fd) {
  Connection &connection = __container_[connection_fd];
  Request    &request    = connection.get_front_request();
  request.send_response(connection_fd);
  if (request.is_send_completed()) {
    if (request.is_close()) {
      shutdown(connection_fd, SHUT_WR);
      request.set_state(CLOSING);
      return;
    }
    connection.erase_front_req();
  }
}
