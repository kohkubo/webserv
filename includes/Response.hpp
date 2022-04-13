#ifndef INCLUDES_RESPONSE_HPP
#define INCLUDES_RESPONSE_HPP

#include <iostream>
#include <map>

#define METHOD      "Method"
#define URL         "URL"
#define VERSION              "HTTP/1.1"
#define STATUS_OK            "200"
#define STATUS_NOTFOUND      "404"
#define TEXT_STATUS_OK       "OK"
#define TEXT_STATUS_NOTFOUND "Not Found"
#define HELLO_WORLD_PAGE     "./html/index.html"
#define ERR_PAGE             "./html/not_found.html"
#define CR                   "\r"
#define LF                   "\n"
#define CRLF                 "\r\n"
#define SP                   " "

class Response {
public:
  Response(const std::string &request_message);

  void        parse();
  void        process();
  std::string message();

private:
  Response();
  Response(Response const &other);
  Response                &operator=(Response const &other);

  std::map<std::string, std::string> __request_;
  std::map<std::string, std::string> __response_;
};

#endif /* INCLUDES_RESPONSE_HPP */