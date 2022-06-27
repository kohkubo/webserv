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

class ResponseGenerator {
public:
  static Response generate_response(
      const RequestInfo &request_info, const bool is_connection_close,
      HttpStatusCode::StatusCode status_code = HttpStatusCode::NONE);

private:
  struct ResponseInfo {
    HttpStatusCode::StatusCode status_code_;
    std::string                target_path_;

    ResponseInfo(HttpStatusCode::StatusCode status_code = HttpStatusCode::NONE,
                 std::string                target_path = "")
        : status_code_(status_code)
        , target_path_(target_path) {}
  };

  ResponseGenerator();
  ~ResponseGenerator();
  static Result<std::string>
                     _read_file_to_str_cgi(const RequestInfo &request_info,
                                           const std::string &target_path);
  static std::string _create_body(const RequestInfo               &request_info,
                                  const HttpStatusCode::StatusCode status_code,
                                  const std::string               &target_path);
  static std::string _create_autoindex_body(const RequestInfo &request_info,
                                            const std::string &target_path);
  static ResponseGenerator::ResponseInfo
  _handle_method(const RequestInfo &request_info);
};

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
