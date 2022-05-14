#include "config/ServerConfig.hpp"
#include "event/Connection.hpp"
#include "utils/utils.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// std::string(std::vector<char>)でnull文字交じりのstring作れるかも

const int buf_size = 2048;

void      connection_receive_handler(int                   accfd,
                                     connection_list_type &connection_list,
                                     socket_list_type     &socket_list) {
  std::cout << "read from fd: " << accfd << std::endl;
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
  connection.make_response(socket_list[connection.socket_fd_]);
}

void connection_send_handler(int accfd, connection_list_type &connection_list) {
  std::cout << "send to fd: " << accfd << std::endl;
  Connection &connection = connection_list[accfd];
  Request    &request    = connection.request_queue_.front();
  ssize_t     sc =
      send(accfd, request.response_.c_str() + request.send_count_,
           request.response_.size() - request.send_count_, MSG_DONTWAIT);

  request.send_count_ += sc;
  // FIXME: c style cast
  if (request.send_count_ == (ssize_t)request.response_.size()) {
    // TODO: 送信完了したrequestがconnection:closeだったら、Connectionを削除。
    connection.request_queue_.pop_front();
    // tmp
    shutdown(accfd, SHUT_WR);
    std::cout << "shutdown fd: " << accfd << std::endl;
    connection_list.erase(accfd);
  }
}
