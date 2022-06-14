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
  static std::string generate_response(const Config      &config,
                                       const RequestInfo &request_info);
  static std::string generate_error_response(const Location &location,
                                             const Config   &config,
                                             HttpStatusCode  status_code);

private:
  ResponseGenerator();
  ~ResponseGenerator();
  static std::string
  __read_file_tostring_cgi(const std::string              &path,
                           const std::vector<std::string> &env);
  static std::string    __response_message(HttpStatusCode     status_code,
                                           const std::string &body,
                                           const Location    &location);
  static std::string    __body(const std::string &file_path,
                               const RequestInfo &request_info);
  static std::string    __create_autoindex_body(const std::string &file_path,
                                                const RequestInfo &request_info);
  static HttpStatusCode __handle_method(const Location    &location,
                                        const RequestInfo &request_info,
                                        const std::string &file_path);
  static HttpStatusCode __handle_post_method(const Location    &location,
                                             const std::string &file_path);
  static HttpStatusCode __handle_get_method(const Location    &location,
                                            const std::string &file_path);
  static HttpStatusCode __handle_delete_method(const Location    &location,
                                               const RequestInfo &request_info,
                                               const std::string &file_path);
};

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */