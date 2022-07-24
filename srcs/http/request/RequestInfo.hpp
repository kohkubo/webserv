#ifndef SRCS_HTTP_REQUEST_REQUESTINFO_HPP
#define SRCS_HTTP_REQUEST_REQUESTINFO_HPP

#include <cstdlib>
#include <map>
#include <stdexcept>
#include <string>

#include "config/Config.hpp"
#include "config/Location.hpp"
#include "http/ContentInfo.hpp"
#include "http/HeaderFieldMap.hpp"
#include "http/HttpStatusCode.hpp"
#include "utils/Path.hpp"
#include "utils/tokenize.hpp"

class RequestInfo {
private:
  static const std::string OWS_;

public:
  typedef std::vector<std::string> transferEncodingVector;
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
    Path        absolute_path_;
    std::string query_;
    std::string http_version_;
  };
  RequestLine             request_line_;
  std::string             host_;
  ContentInfo             content_info_;
  std::string             connection_;
  transferEncodingVector  transfer_encoding_;
  config::Config          config_;
  const config::Location *location_;

public:
  RequestInfo()
      : location_(NULL) {}
  ~RequestInfo() {
    if (location_ == NULL) {
      delete location_;
    }
  }
  class BadRequestException : public std::logic_error {
  private:
    HttpStatusCode _status_;

  public:
    BadRequestException(
        HttpStatusCode     status = HttpStatusCode::S_400_BAD_REQUEST,
        const std::string &msg    = "Illegal request.");
    HttpStatusCode status() const;
  };

  bool has_body() const {
    return content_info_.has_content_length() || is_chunked();
  }
  bool is_chunked() const {
    return transfer_encoding_.size() != 0 &&
           transfer_encoding_.back() == "chunked";
  }
  bool is_close_connection() const { return connection_ == "close"; }
  void parse_request_line(const std::string &request_line);
  void parse_request_header(const HeaderFieldMap &header_field_map);
  bool is_valid_request_header() const;
};

#endif /* SRCS_HTTP_REQUEST_REQUESTINFO_HPP */
