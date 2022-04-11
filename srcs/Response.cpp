#include "Response.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define STATUS_OK         "200"
#define TEXT_STATUS_OK    "OK"
#define CR                "\r"
#define LF                "\n"
#define CRLF              "\r\n"
#define SP                " "
#define CONTENT_LENGTH    "Content-Length: "
#define CONTENT_TYPE_HTML "Content-Type: text/html"
#define CONNECTION_CLOSE  "Connection: close"

/*
 * recvで受け取った文字列をもとにリクエストを作る予定です。
 * 今は決めうちで作ってます。
 */
Response::Response(const std::string &message) {
  // lexer(message)
  (void)message;

  // リクエスト1行目
  __request_line_.push_back("GET");
  __request_line_.push_back("./html/index.html");
  __request_line_.push_back("HTTP/1.1");

  // フィールド
  __request_field_.push_back("Host: example.com");
  __request_field_.push_back("User-Agent: curl/7.79.1");
  __request_field_.push_back("Accept: */*");

  // ボディ省略
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

// void Response::method_get(std::string file_path) {

//	this->__version_ = HTTP;
//	this->__status_ = STATUS_OK;
//	this->__phrase_ = PHRASE_STATUS_OK;

//	std::ifstream instream(file_path);
//	if (instream.fail())
//		exit(1);
//	std::stringstream buf;
//	buf << instream.rdbuf();
//	std::string content = buf.str();
//	this->__body_ = content;
//	this->__content_len_ = CONTENT_LENGTH + std::to_string(content.size());
//	this->__content_type_ = CONTENT_TYPE_HTML;

//	this->__connection_ = CONNECTION_CLOSE;

//	return ;
//}

// std::string Response::response_message() {
//	return	this->__version_ + SP + std::to_string(this->__status_) + SP +
//this->__phrase_ + CRLF
//			+ this->__content_type_ + CRLF
//			+ this->__content_len_ + CRLF
//			+ this->__connection_ + CRLF
//			+ CRLF
//			+ this->__body_;
// }