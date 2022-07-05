#ifndef SRCS_HTTP_RESPONSE_CONTENT_HPP
#define SRCS_HTTP_RESPONSE_CONTENT_HPP

#include <string>

#include "http/HttpStatusCode.hpp"

typedef int fileFd;

namespace response_generator {

class Content {
public:
  enum Action {
    READ,    // FD読み込み待ち
    WRITE,   // FD書き込み待ち
    CREATED, // contentを作成済み
    CGI      // CGI
  };
  Action         action_;
  HttpStatusCode status_code_;
  std::string    content_;
  fileFd         fd_;
  pid_t          cgi_pid_;

public:
  Content(HttpStatusCode status_code = HttpStatusCode::S_200_OK);
  Content(int fd, Action action,
          HttpStatusCode status_code = HttpStatusCode::S_200_OK);
  Content(int fd, Action action, pid_t cgi_pid);
  Content(std::string content, HttpStatusCode status_code,
          Action action = CREATED);
};

} // namespace response_generator

#endif /* SRCS_HTTP_RESPONSE_CONTENT_HPP */
