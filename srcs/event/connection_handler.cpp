#include "config/ServerConfig.hpp"
#include "event/Connection.hpp"
#include "utils/utils.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// std::string(std::vector<char>)でnull文字交じりのstring作れるかも

const int buf_size = 2048;

void      connection_receive_handler(int                   accfd,
                                     connection_list_type &connection_list) {
  Connection       &connection = connection_list[accfd];
  std::vector<char> buf(buf_size);
  ssize_t           rc = recv(accfd, &buf[0], buf_size, MSG_DONTWAIT);
  if (rc == -1) {
    error_log_with_errno("recv() failed.");
  }
  if (rc == 0) {
    std::cerr << "closed from client." << std::endl;
    close(accfd);
    connection_list.erase(accfd);
    return;
  }
  std::string data = std::string(buf.begin(), buf.begin() + rc);
  connection.parse_buffer(data);
  // cgi用のstateが必要になるかも
  connection.create_response_iter();
}

void connection_send_handler(int accfd, connection_list_type &connection_list) {
  Connection &connection = connection_list[accfd];
  Request    &request    = connection.get_front_request();
  request.send_response(accfd);
  if (request.is_send_completed()) {
    if (request.info_.is_close_ == true) {
      shutdown(accfd, SHUT_WR);
      request.set_state(CLOSING);
      return;
    }
    connection.erase_front_req();
  }
}
