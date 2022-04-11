#include "Request.hpp"

#define HTML_PATH "./html/index.html"
#define METHOD    "GET"
#define VERSION   "HTTP/1.1"

/*
 * recvで受け取った文字列をもとにパースする予定です。
 * 今は決めうちで書いてます。
 */
Request::Request(const std::string &message) {
  // lexer(message)
  (void)message;
  __method_  = METHOD;
  __uri_     = HTML_PATH;
  __version_ = VERSION;
  __fields_.push_back("Host: example.com");
  __fields_.push_back("User-Agent: curl/7.79.1");
  __fields_.push_back("Accept: */*");
}

Request::Request(Request const &other) { *this = other; }

Request &Request::operator=(Request const &other) {
  if (this != &other) {
    /* copy */
  }
  return *this;
}
