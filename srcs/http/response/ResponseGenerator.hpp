#ifndef SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP
#define SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP

#include <algorithm>

#include "config/Config.hpp"
#include "http/const/const_delimiter.hpp"
#include "http/const/const_header_field_values.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/utils.hpp"

class ResponseGenerator {
public:
  static std::string generate_response(const Config      &config,
                                       const RequestInfo &request_info);

private:
  ResponseGenerator();
  ~ResponseGenerator();
  static std::string
  __read_file_tostring_cgi(const std::string              &path,
                           const std::vector<std::string> &env);
  static const Location                       *
  __select_proper_location(const std::string           &request_uri,
                                                 const std::vector<Location> &locations);

  static std::string    __response_message(HttpStatusCode     status_code,
                                           const std::string &body);
  static std::string    __file_path(const std::string &request_uri,
                                    const Location    &location);
  static bool           __is_error_status_code(HttpStatusCode status_code);
  static HttpStatusCode __check_filepath_status(const Location    &location,
                                                const std::string &file_path);
  static std::string    __error_page_body(const Location      &location,
                                          const Config        &config,
                                          const HttpStatusCode status_code);
  static std::string    __body(const std::string &file_path,
                               const RequestInfo  request_info);
  static std::string    __create_autoindex_body(const std::string &file_path,
                                                const RequestInfo  request_info);

  static HttpStatusCode __handle_post_method(const Location   &location,
                                             const std::string file_path) {
    // TODO: ここらへんの処理、未定なので雑に書いています。
    if (std::find(location.limit_except_.begin(), location.limit_except_.end(),
                  "POST") == location.limit_except_.end()) {
      return NOT_ALLOWED_405;
    }
    return __check_filepath_status(location, file_path);
  }

  static HttpStatusCode __handle_get_method(const Location   &location,
                                            const std::string file_path) {
    // TODO: ここらへんの処理、未定なので雑に書いています。
    if (std::find(location.limit_except_.begin(), location.limit_except_.end(),
                  "GET") == location.limit_except_.end()) {
      return NOT_ALLOWED_405;
    }
    return __check_filepath_status(location, file_path);
  }

  static HttpStatusCode __delete_target_file(const RequestInfo request_info,
                                             const std::string file_path);

  static HttpStatusCode __handle_delete_method(const Location   &location,
                                               const RequestInfo request_info,
                                               const std::string file_path) {
    // TODO: ここらへんの処理、未定なので雑に書いています。
    if (std::find(location.limit_except_.begin(), location.limit_except_.end(),
                  "DELETE") == location.limit_except_.end()) {
      return NOT_ALLOWED_405;
    }
    return __delete_target_file(request_info, file_path);
  }
};

std::map<int, std::string> init_response_status_phrase_map();
std::map<int, std::string> init_page_contents_map();

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
