#ifndef SRCS_HTTP_HTTPMESSAGE_HPP
#define SRCS_HTTP_HTTPMESSAGE_HPP

#include <map>
#include <string>

// method
enum HttpMethod { GET, POST, DELETE, UNKNOWN };
enum HttpStatusCode {
  NONE = -1,
  OK_200,
  NO_CONTENT_204,
  BAD_REQUEST_400,
  FORBIDDEN_403,
  NOT_FOUND_404,
  INTERNAL_SERVER_ERROR_500,
  NOT_IMPLEMENTED_501,
  UNKNOWN_ERROR_520,
};

struct HttpMessage {
  HttpMethod                         method_;
  std::string                        url_;
  std::string                        path_;
  std::string                        version_;
  std::string                        host_;
  std::string                        status_;
  std::string                        phrase_;
  size_t                             content_length_;
  std::string                        body_;
  // TODO: values_ CGIにわたす形式に合わせる。仮置でmap
  std::map<std::string, std::string> values_;
  HttpStatusCode                     status_code_;
  // FIXME: 仮置きです。
  bool                               is_close_;

  HttpMessage()
      : method_(UNKNOWN)
      , content_length_(0)
      , status_code_(NONE)
      // tmp
      , is_close_(true) {}

  bool is_expected_body() {
    // TODO: chunked
    return content_length_ != 0;
  }
};

#endif /* SRCS_HTTP_HTTPMESSAGE_HPP */
