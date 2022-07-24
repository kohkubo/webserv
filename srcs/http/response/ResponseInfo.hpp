#ifndef SRCS_HTTP_RESPONSE_RESPONSEINFO_HPP
#define SRCS_HTTP_RESPONSE_RESPONSEINFO_HPP

#include <string>

#include "http/HttpStatusCode.hpp"

typedef int fileFd;

namespace response_generator {

class ResponseInfo {
public:
  enum Action {
    READ,    // FD読み込み待ち
    WRITE,   // FD書き込み待ち
    CREATED, // contentを作成済み
    CGI      // CGI
  };
  Action         action_;
  std::string    content_;
  HttpStatusCode status_code_;
  fileFd         fd_;
  pid_t          cgi_pid_;

public:
  ResponseInfo()
      : action_(READ)
      , status_code_(HttpStatusCode::S_200_OK)
      , fd_(-1)
      , cgi_pid_(0) {}
  ResponseInfo(Action action, std::string content, HttpStatusCode status_code,
               int fd = -1, pid_t pid = 0)
      : action_(action)
      , content_(content)
      , status_code_(status_code)
      , fd_(fd)
      , cgi_pid_(pid) {}
};

} // namespace response_generator

#endif /* SRCS_HTTP_RESPONSE_RESPONSEINFO_HPP */
