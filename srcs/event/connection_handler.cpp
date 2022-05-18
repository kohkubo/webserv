#include "config/ServerConfig.hpp"
#include "event/Connection.hpp"
#include "utils/utils.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// std::string(std::vector<char>)でnull文字交じりのstring作れるかも

const int buf_size = 2048;

void      connection_receive_handler(int                            accfd,
                                     std::map<conn_fd, Connection> &conn_fd_map) {
  Connection       &connection = conn_fd_map[accfd];
  std::vector<char> buf(buf_size);
  ssize_t           rc = recv(accfd, &buf[0], buf_size, MSG_DONTWAIT);
  switch (rc) {
  case -1:
    error_log_with_errno("recv() failed.");
    break;
  case 0:
    std::cerr << "closed from client." << std::endl;
    close(accfd);
    conn_fd_map.erase(accfd);
    return;
  default:
    std::string data = std::string(buf.begin(), buf.begin() + rc);
    connection.parse_buffer(data);
    // cgi用のstateが必要になるかも
    connection.create_response_iter();
    break;
  }
}

void connection_send_handler(int                            accfd,
                             std::map<conn_fd, Connection> &conn_fd_map) {
  Connection &connection = conn_fd_map[accfd];
  Request    &request    = connection.get_front_request();
  request.send_response(accfd);
  if (request.is_send_completed()) {
    if (request.is_close()) {
      shutdown(accfd, SHUT_WR);
      request.set_state(CLOSING);
      return;
    }
    connection.erase_front_req();
  }
}
