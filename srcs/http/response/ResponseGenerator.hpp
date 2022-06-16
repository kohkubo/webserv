#ifndef SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP
#define SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP

#include <algorithm>

#include "config/Config.hpp"
#include "http/const/const_delimiter.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/utils.hpp"

class ResponseGenerator {
public:
  static std::string generate_response(const RequestInfo &request_info);
  static std::string generate_error_response(const RequestInfo &request_info,
                                             HttpStatusCode     status_code);

private:
  ResponseGenerator();
  ~ResponseGenerator();
  static std::string    _read_file_to_str_cgi(const std::string &path,
                                              const RequestInfo &request_info);
  static std::string    _body(const std::string &file_path,
                              const RequestInfo &request_info);
  static std::string    _create_autoindex_body(const std::string &file_path,
                                               const RequestInfo &request_info);
  static HttpStatusCode _handle_method(const Location    &location,
                                       const RequestInfo &request_info,
                                       const std::string &file_path);
};

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */