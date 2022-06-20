#ifndef SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP
#define SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP

#include <algorithm>

#include "config/Config.hpp"
#include "http/const/const_delimiter.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

class ResponseGenerator {
public:
  static std::string generate_response(const RequestInfo &request_info);
  static std::string
  generate_error_response(const RequestInfo         &request_info,
                          HttpStatusCode::StatusCode status_code);

private:
  ResponseGenerator();
  ~ResponseGenerator();
  static Result      _read_file_to_str_cgi(const RequestInfo &request_info);
  static std::string _body(const RequestInfo &request_info);
  static std::string _create_autoindex_body(const RequestInfo &request_info);
  static HttpStatusCode::StatusCode
  _handle_method(const RequestInfo &request_info);
};

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
