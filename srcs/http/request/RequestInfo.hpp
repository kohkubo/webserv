#ifndef SRCS_HTTP_REQUEST_REQUESTINFO_HPP
#define SRCS_HTTP_REQUEST_REQUESTINFO_HPP

#include <map>
#include <stdexcept>
#include <string>

#include "config/Config.hpp"
#include "config/Location.hpp"
#include "http/const/const_http_enums.hpp"
#include "utils/tokenize.hpp"

class RequestInfo {
private:
  static const std::string OWS_;

public:
  /*
  RFC 9112
  https://triple-underscore.github.io/http1-ja.html#p.absolute-form
  request-line
    = method SP request-target SP HTTP-version

  request-target
  = origin-form
  / absolute-form
  / authority-form
  / asterisk-form

  今回使うのはorigin-formだけ
  origin-form
  = absolute-path [ "?" query ]
  */
  struct RequestLine {
    std::string method_;
    std::string absolute_path_;
    std::string query_;
    std::string http_version_;
  };
  RequestLine     request_line_;
  std::string     host_;
  std::string     body_;
  bool            connection_close_;
  bool            is_chunked_;
  bool            has_content_length_;
  std::size_t     content_length_;
  Config          config_;
  const Location *location_;

public:
  RequestInfo()
      : connection_close_(false)
      , is_chunked_(false)
      , has_content_length_(false)
      , content_length_(0) {}

  class BadRequestException : public std::logic_error {
  private:
    HttpStatusCode::StatusCode _status_;

  public:
    BadRequestException(
        HttpStatusCode::StatusCode status = HttpStatusCode::BAD_REQUEST_400,
        const std::string         &msg    = "Illegal request.");
    HttpStatusCode::StatusCode status() const;
  };

  bool has_body() const { return has_content_length_ || is_chunked_; }

  static void store_request_header_field_map(
      const std::string                  &header_line,
      std::map<std::string, std::string> &header_field_map);
  void parse_request_line(const std::string &request_line);
  void parse_request_header(
      const std::map<std::string, std::string> &header_field_map);
  bool is_valid_request_header() const;
};

#endif /* SRCS_HTTP_REQUEST_REQUESTINFO_HPP */
