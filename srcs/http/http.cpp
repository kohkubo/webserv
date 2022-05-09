#include "config/ServerConfig.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/request/request.hpp"
#include "http/response/response.hpp"
#include "utils/utils.hpp"
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
  // TODO: 適切なServerConfigが渡される。
  const ServerConfig       server_config  = ServerConfig();
  std::vector<std::string> request_tokens = receive_request(accfd);
  HttpMessage              request_info = parse_request_message(request_tokens);
  http_message_map         response_info =
      create_response_info(server_config, request_info);
  std::string response_message = make_message_string(response_info);
  send_response(accfd, response_message);
}

/*

とりあえず、の流れ
receive_massageはリクエストとbodyが全て読み込まれた状態。
（リクエスト、bodyを別々に持ったほうが良いかも。要検討）

recieve_request -> accfdからの読み込み、トークンとして返す std::vector<str>
parse_request_message -> messageをパース HttpMessage
create_response_message -> パースしたメッセージからレスポンスを生成 std::string
send_response -> レスポンスを送信

*/