#include "config/ServerConfig.hpp"
#include "event/Connection.hpp"
#include "utils/utils.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// std::string(std::vector<char>)でnull文字交じりのstring作れるかも

static std::string receive_data(int accfd) {
  const int         buf_size = 1;
  std::vector<char> buf(buf_size);
  ssize_t           rc = recv(accfd, &buf[0], buf_size, MSG_DONTWAIT);
  if (rc == -1) {
    error_log_with_errno("recv() failed.");
    return std::string();
  }
  // TODO: rc == 0 => connection close
  return std::string(buf.begin(), buf.begin() + rc);
}

void connection_receive_handler(int                   accfd,
                                connection_list_type &connection_list,
                                socket_list_type     &socket_list) {
  Connection &connection = connection_list[accfd];
  std::string data       = receive_data(accfd);
  connection.parse_buffer(data);
  // cgi用のstateが必要になるかも
  connection.make_response(socket_list[connection.socket_fd_]);
}

void connection_send_handler(int accfd, connection_list_type &connection_list) {
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
    close(accfd);
    connection_list.erase(accfd);
  }
}