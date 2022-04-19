#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "Lexer.hpp"
#include "http.hpp"
#include "util.hpp"

#define BUF_SIZE 1024

/*
 * メッセージ読み込み
 */
static std::string http_receive_request(int accfd) {
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
  std::cout << "http_receive_request()" << std::endl;
  std::cout << recv_str << std::endl;
  return recv_str;
}

/*
 * メッセージ書き込み
 */
static void http_send_response(int accfd, const std::string &message) {
  send(accfd, message.c_str(), message.size(), 0);
  std::cout << "http_send_response()" << std::endl;
  std::cout << message << std::endl;
}

static HttpMethod
http_request_message_method_to_int(const std::string &method) {
  if (method == "GET") {
    return GET;
  }
  if (method == "POST") {
    return POST;
  }
  if (method == "DELETE") {
    return DELETE;
  }
  return UNKNOWN;
}

/*
 * リクエストを受けて, レスポンスを返すまでの処理
 */
void http(int accfd) {
  Lexer            request_lexer(http_receive_request(accfd), SP);
  http_message_map request_message = http_parse_request_message(request_lexer);
  http_message_map response_message;

  switch (http_request_message_method_to_int(request_message[METHOD])) {
  case GET:
    response_message = http_read_method_get(request_message);
    break;
  // case POST:
  //   break;
  // case DELETE:
  //   break;
  default:
    break;
  }
  request_message[VERSION]    = VERSION_HTTP;     // TODO: 別関数に実装
  request_message[CONNECTION] = CONNECTION_CLOSE; // TODO: 別関数に実装
  http_send_response(accfd, http_response_message_get(response_message));
}