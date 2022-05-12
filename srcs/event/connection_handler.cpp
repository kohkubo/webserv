#include "config/ServerConfig.hpp"
#include "event/Connection.hpp"
#include "utils/utils.hpp"
#include <sys/socket.h>
#include <sys/types.h>

// std::string(std::vector<char>)でnull文字交じりのstring作れるかも

static std::string receive_data(int accfd) {
  const int buf_size      = 1024;
  char      buf[buf_size] = {};
  ssize_t   rc            = recv(accfd, buf, buf_size - 1, MSG_DONTWAIT);
  if (rc == -1) {
    error_log_with_errno("recv() failed.");
    return std::string();
  }
  // TODO: rc == 0 => connection close
  return std::string(buf);
}

void connection_handler(int accfd, connection_list_type &connection_list,
                        const socket_list_type &socket_list) {
  // TODO: 適切なServerConfigを選択する。
  (void)socket_list;

  Connection &connection = connection_list[accfd];
  std::string data       = receive_data(accfd);
  connection.parse_buffer(data);
  // pendingのリクエストをすべてレスポンス作成に渡す。(pending->wating)　cgiのstateひつようかも
  // listen_eventでレスポンスを送信キューに追加する。
}