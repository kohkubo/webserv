#include "message.hpp"
#include "util.hpp"
#include <iostream>
#include <map>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024

std::string read_request(int accfd) {
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
  std::cout << "read_request()" << std::endl;
  std::cout << recv_str << std::endl;
  return recv_str;
}

void send_response(int accfd, const std::string &message) {
  send(accfd, message.c_str(), message.size(), 0);
  std::cout << "send_response()" << std::endl;
  std::cout << message << std::endl;
}

/*
 * ステータスラインの要素は必須だが, 存在しなかった時のバリデートは現状してない
 */
static std::string status_line_message(message_type &response) {
  return response[VERSION] + SP + response[STATUS] + SP + response[PHRASE] +
         CRLF;
}

static std::string field_line(message_type &response, const std::string key) {
  if (response.find(key) == response.end())
    return "";
  else
    return key + ": " + response[key] + CRLF;
}

static std::string field_message(message_type &response) {
  return field_line(response, HOST) + field_line(response, CONTENT_LEN) +
         field_line(response, CONTENT_TYPE) + field_line(response, CONNECTION);
}

static std::string body_message(message_type &response) {
  return response[BODY];
}

/*
 * 引数responseの型は const message_type& で受け取りたかったが
 * map[key]でのアクセス方法はconstに用意されていないみたいなので現状このまま
 */
std::string response_message(message_type &response) {
  return status_line_message(response) + field_message(response) + CRLF +
         body_message(response);
}
