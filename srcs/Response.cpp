#include "Response.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define STATUS_OK      "200"
#define TEXT_STATUS_OK "OK"
#define VERSION        "HTTP/1.1"
#define CR             "\r"
#define LF             "\n"
#define CRLF           "\r\n"
#define SP             " "

/*
 * recvでクライアントから受け取ったメッセージデータをもとに
 * リクエストを作る予定です。
 * 今は決めうちで作ってます。
 */
Response::Response(const std::string &request_message) {
  // lexer(request_message)
  (void)request_message;

  __request_line_.push_back("GET");
  __request_line_.push_back("./html/index.html");
  __request_line_.push_back(VERSION);

  __request_field_.push_back("Host: example.com");
  __request_field_.push_back("User-Agent: curl/7.79.1");
  __request_field_.push_back("Accept: */*");
}

Response::Response(Response const &other) { *this = other; }

Response &Response::operator=(Response const &other) {
  if (this != &other) {
    this->__request_line_   = other.__request_line_;
    this->__request_field_  = other.__request_field_;
    this->__request_body_   = other.__request_body_;
    this->__status_line_    = other.__status_line_;
    this->__response_field_ = other.__response_field_;
    this->__response_body_  = other.__response_body_;
  }
  return *this;
}

/*
 * レスポンスに必要な属性を埋める.
 */
void Response::process() {

  __status_line_.push_back(VERSION);
  __status_line_.push_back(STATUS_OK);
  __status_line_.push_back(TEXT_STATUS_OK);

  // リクエストのターゲット(index.html)から内容読み込み, セット
  std::string file_path    = __request_line_[1];
  std::string content      = read_file(file_path);
  std::string content_size = sizettos(content.size());
  __response_body_         = content;
  __response_field_.push_back("Content-Length: " + content_size);

  __response_field_.push_back("Content-Type: text/html");
  __response_field_.push_back("Connection: close");
}

/*
 * size_t -> string
 */
std::string sizettos(size_t val) {
  // size_tの桁数 + '\0' 分のbufferを用意
  char buffer[std::numeric_limits<size_t>::digits10 + 1 + 1];
  std::sprintf(buffer, "%zu", val);
  return buffer;
}

std::string read_file(const std::string &path) {
  std::ifstream file(path);
  if (file.fail()) {
    std::cout << "fail to open file" << std::endl;
    exit(1);
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

std::string Response::message() {
  return __status_line_[0] + SP + __status_line_[1] + SP + __status_line_[2] +
         CRLF + __response_field_[0] + CRLF + __response_field_[1] + CRLF +
         __response_field_[2] + CRLF + CRLF + __response_body_;
}