#ifndef SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP
#define SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP

#include <algorithm>

#include "config/Config.hpp"
#include "event/Response.hpp"
#include "http/const/const_delimiter.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

typedef int fileFd;

class ResponseGenerator {
public:
  struct Body {
    fileFd      fd_;
    std::string content_;

    Body()
        : fd_(-1) {}
    bool has_fd() const { return fd_ != -1; }
  };

private:
  struct ResponseInfo {
    const RequestInfo         &request_info_;
    HttpStatusCode::StatusCode status_code_;
    bool                       is_connection_close_;
    ResponseInfo(const RequestInfo         &request_info,
                 HttpStatusCode::StatusCode status_code)
        : request_info_(request_info)
        , status_code_(status_code)
        , is_connection_close_(
              request_info_.connection_close_ ||
              status_code_ == HttpStatusCode::BAD_REQUEST_400 ||
              status_code_ == HttpStatusCode::ENTITY_TOO_LARGE_413) {}
  };
  ResponseInfo _response_info_;

public:
  ResponseGenerator(
      const RequestInfo         &request_info,
      HttpStatusCode::StatusCode status_code = HttpStatusCode::NONE)
      : _response_info_(request_info, status_code) {}
  ~ResponseGenerator() {}
  Response generate_response();

private:
  static Result<std::string>
                     _read_file_to_str_cgi(const RequestInfo &request_info);
  static Body        _create_body(const RequestInfo               &request_info,
                                  const HttpStatusCode::StatusCode status_code);
  static std::string _create_autoindex_body(const RequestInfo &request_info);
  static HttpStatusCode::StatusCode
  _handle_method(const RequestInfo &request_info);
  static HttpStatusCode::StatusCode
                     _get_status_code(const RequestInfo         &request_info,
                                      HttpStatusCode::StatusCode status_code);
  static std::string _create_response_message(
      const RequestInfo &request_info, const bool is_connection_close,
      HttpStatusCode::StatusCode status_code, const std::string &body);
  static Body _body_of_status_code(const RequestInfo         &request_info,
                                   HttpStatusCode::StatusCode status_code);
};

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
