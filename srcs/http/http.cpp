#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "config/ServerConfig.hpp"
#include "http.hpp"
#include "util.hpp"

/*
 * メッセージ書き込み
 */
void send_response(int accfd, const std::string &message) {
  send(accfd, message.c_str(), message.size(), 0);
  std::cout << "send_response()" << std::endl;
  std::cout << message << std::endl;
}

/*
 * リクエストを受けて, レスポンスを返すまでの処理
 */
void http(int accfd) {
  ServerConfig server_config;
  http_message_map request_message = receive_request(server_config, accfd);
  std::string      response_message =
      create_response(server_config, request_message);
  send_response(accfd, response_message);
}
