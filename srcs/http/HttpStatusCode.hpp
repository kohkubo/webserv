#ifndef SRCS_HTTP_HTTPSTATUSCODE_HPP
#define SRCS_HTTP_HTTPSTATUSCODE_HPP

#include <string>

class HttpStatusCode {
public:
  enum StatusCode {
    S_200_OK                    = 200,
    S_201_CREATED               = 201,
    S_204_NO_CONTENT            = 204,
    S_301_MOVED_PERMANENTLY     = 301,
    S_302_MOVED_TEMPORARILY     = 302,
    S_304_NOT_MODIFIED          = 304,
    S_400_BAD_REQUEST           = 400,
    S_403_FORBIDDEN             = 403,
    S_404_NOT_FOUND             = 404,
    S_405_NOT_ALLOWED           = 405,
    S_413_ENTITY_TOO_LARGE      = 413,
    S_500_INTERNAL_SERVER_ERROR = 500,
    S_501_NOT_IMPLEMENTED       = 501,
    S_504_GATEWAY_TIME_OUT      = 504,
    S_520_UNKNOWN_ERROR         = 520,
  };

  StatusCode status_code_;

public:
  HttpStatusCode(StatusCode status_code = S_200_OK);
  HttpStatusCode(const int status_num);
  HttpStatusCode &operator=(const HttpStatusCode &other);
  HttpStatusCode &operator=(StatusCode status_code);
  HttpStatusCode &operator=(const int num);
  bool            is_error_status_code() const;
  bool            has_default_content() const;
  std::string     status_phrase() const;
  std::string     str() const;
  std::string     create_default_content_str() const;
                  operator StatusCode() const;
};

#endif /* SRCS_HTTP_HTTPSTATUSCODE_HPP */
