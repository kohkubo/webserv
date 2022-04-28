#include "Http.hpp"
#include "process_http.hpp"
#include "ServerConfig.hpp"
#include "util.hpp"
#include <string>
#include <sys/socket.h>
#include <unistd.h>

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
void process_http(int accfd) {
  const ServerConfig server_config;
  HttpMessage        request_message = receive_request(accfd);
  std::string        response_message =
      create_response(server_config, request_message);
  send_response(accfd, response_message);
}
