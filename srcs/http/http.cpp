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

void init_server_config(ServerConfig &server_config) {
  server_config.root_ = "./html/";
  server_config.index_.push_back("index.html");
  server_config.index_.push_back("index.htm");
  server_config.status_code_ = 0;
}

/*
 * リクエストを受けて, レスポンスを返すまでの処理
 */
void http(int accfd) {
  ServerConfig server_config;
  init_server_config(server_config);
  http_message_map request_message = receive_request(server_config, accfd);
  std::string      response_message =
      create_response(server_config, request_message);
  send_response(accfd, response_message);
}
