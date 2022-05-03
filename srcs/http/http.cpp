#include "config/ServerConfig.hpp"
#include "request.hpp"
#include "response.hpp"
#include "util/util.hpp"
#include <string>
#include <sys/socket.h>
#include <unistd.h>

/*
 * メッセージ書き込み
 */
static void send_response(int accfd, const std::string &message) {
  send(accfd, message.c_str(), message.size(), 0);
  std::cout << "send_response()" << std::endl;
  std::cout << message << std::endl;
}

/*
 * リクエストを受けて, レスポンスを返すまでの処理
 */
void http(int accfd) {
  const ServerConfig server_config   = ServerConfig();
  HttpMessage        request_message = receive_request(accfd);
  std::string        response_message =
      create_response(server_config, request_message);
  send_response(accfd, response_message);
}
