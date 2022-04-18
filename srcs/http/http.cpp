#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "Lexer.hpp"
#include "http/define.hpp"
#include "http/method.hpp"
#include "http/request.hpp"
#include "http/response.hpp"
#include "util.hpp"

#define BUF_SIZE 1024

/*
 * メッセージ読み込み
 */
static std::string read_message(int accfd) {
  char        buf[BUF_SIZE] = {};
  std::string recv_str      = "";
  ssize_t     read_size     = 0;

  do {
    read_size = recv(accfd, buf, sizeof(buf) - 1, 0);
    if (read_size == -1) {
      error_log_with_errno("read() failed.");
      close(accfd);
      // accfd = -1;
      break;
    }
    if (read_size > 0) {
      recv_str.append(buf);
    }
    if (is_match_suffix_string(recv_str, "\r\n\r\n")) {
      break;
    }
  } while (read_size > 0);
  std::cout << "read_message()" << std::endl;
  std::cout << recv_str << std::endl;
  return recv_str;
}

/*
 * メッセージ書き込み
 */
static void send_message(int accfd, const std::string &message) {
  send(accfd, message.c_str(), message.size(), 0);
  std::cout << "send_message()" << std::endl;
  std::cout << message << std::endl;
}

/*
 * リクエストを受けて, レスポンスを返すまでの処理
 */
void http(int accfd) {
  Lexer        request_lexer(read_message(accfd), SP);
  message_type request = parse_request(request_lexer);
  message_type response;
  get(request, response);
  response[VERSION]    = VERSION_HTTP;     // TODO: 別関数に実装
  response[CONNECTION] = CONNECTION_CLOSE; // TODO: 別関数に実装
  send_message(accfd, response_message(response));
}
