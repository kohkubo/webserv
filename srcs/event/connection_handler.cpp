#include "config/ServerConfig.hpp"
#include "event/Connection.hpp"
#include "utils/utils.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// std::string(std::vector<char>)でnull文字交じりのstring作れるかも

const int buf_size = 2048;

void      connection_receive_handler(int                        conn_fd,
                                     std::map<int, Connection> &conn_fd_map) {
  Connection       &connection = conn_fd_map[conn_fd];
  std::vector<char> buf(buf_size);
  ssize_t           rc = recv(conn_fd, &buf[0], buf_size, MSG_DONTWAIT);
  switch (rc) {
  case -1:
    error_log_with_errno("recv() failed.");
    break;
  case 0:
    std::cerr << "closed from client." << std::endl;
    close(conn_fd);
    conn_fd_map.erase(conn_fd);
    return;
  default:
    std::string data = std::string(buf.begin(), buf.begin() + rc);
    connection.parse_buffer(data);
    // cgi用のstateが必要になるかも
    connection.create_response_iter();
    break;
  }
}

void connection_send_handler(int                        conn_fd,
                             std::map<int, Connection> &conn_fd_map) {
  Connection &connection = conn_fd_map[conn_fd];
  Response   &response   = connection.get_front_response();
  response.send_message(conn_fd);
  if (response.is_send_completed()) {
    if (response.is_close()) {
      shutdown(conn_fd, SHUT_WR);
      response.set_closing();
      return;
    }
    connection.erase_front_response();
  }
}
