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
    bool        has_fd_;
    fileFd      fd_;
    std::string content_;

    Body()
        : has_fd_(false)
        , fd_(-1)
        , content_("") {}
  };
  static Response generate_response(
      const RequestInfo &request_info, const bool is_connection_close,
      HttpStatusCode::StatusCode status_code = HttpStatusCode::NONE);

private:
  struct ResponseInfo {
    HttpStatusCode::StatusCode status_code_;
    Body                       body_;

    ResponseInfo(HttpStatusCode::StatusCode status_code = HttpStatusCode::NONE,
                 Body                       body        = Body())
        : status_code_(status_code)
        , body_(body) {}
  };
  ResponseGenerator();
  ~ResponseGenerator();
  static Result<std::string>
              _read_file_to_str_cgi(const RequestInfo &request_info);
  static Body _create_body(const RequestInfo               &request_info,
                           const HttpStatusCode::StatusCode status_code);
  static bool _is_error_status_code(HttpStatusCode::StatusCode status_code);
  static std::string _create_autoindex_body(const RequestInfo &request_info);
  static ResponseGenerator::Body
  _body_of_status_code(const RequestInfo         &request_info,
                       HttpStatusCode::StatusCode status_code);
  static ResponseGenerator::ResponseInfo
  _handle_method(const RequestInfo &request_info);
  static ResponseInfo
  _get_statuscode_body(const RequestInfo         &request_info,
                       HttpStatusCode::StatusCode status_code);
};

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
