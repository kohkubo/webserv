#ifndef SRCS_HTTP_RESPONSE_RESPONSE_HPP
#define SRCS_HTTP_RESPONSE_RESPONSE_HPP

#include <algorithm>

#include "config/Config.hpp"
#include "http/const/const_delimiter.hpp"
#include "http/const/const_header_field_values.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/utils.hpp"

class Response {
private:
  const Config      &__config_;
  const RequestInfo &__request_info_;
  HttpStatusCode     __status_code_;
  std::string        __response_string_;
  std::string        __file_path_;

  std::string        __status_phrase_;
  std::string        __content_len_;
  std::string        __content_type_;
  std::string        __connection_;
  std::string        __body_;

public:
  Response(const Config &config, const RequestInfo &request_info);
  virtual ~Response(){};
  std::string get_response_string();

private:
  Response();
  static std::string
  __read_file_tostring_cgi(const std::string              &path,
                           const std::vector<std::string> &env);
  static const Location             *
  __get_proper_location(const std::string           &request_uri,
                                    const std::vector<Location> &locations);

  void        __set_status_phrase();
  void        __set_general_header();
  void        __set_entity_header();
  void        __set_content_len();
  void        __set_content_type();
  void        __make_message_string();
  void        __make_bodiless_message_string();

  void        __set_file_path(const std::string &request_uri,
                              const Location    &location);
  bool        __is_error_status_code();
  void        __check_filepath_status(const Location &location);
  void        __set_error_page_body(const Location &location);
  void        __set_body();
  std::string __create_autoindex_body(const std::string &file_path);

  void        __post_method_handler(const Location &location) {
    // TODO: ここらへんの処理、未定なので雑に書いています。
    if (std::find(location.limit_except_.begin(), location.limit_except_.end(),
                         "POST") == location.limit_except_.end()) {
      __status_code_ = NOT_ALLOWED_405;
      return;
      __check_filepath_status(location);
    }
  }
  void __get_method_handler(const Location &location) {
    // TODO: ここらへんの処理、未定なので雑に書いています。
    if (std::find(location.limit_except_.begin(), location.limit_except_.end(),
                  "GET") == location.limit_except_.end()) {
      __status_code_ = NOT_ALLOWED_405;
      return;
    }
    __check_filepath_status(location);
  }
  void __delete_target_file();
  void __delete_method_handler(const Location &location) {
    // TODO: ここらへんの処理、未定なので雑に書いています。
    if (std::find(location.limit_except_.begin(), location.limit_except_.end(),
                  "DELETE") == location.limit_except_.end()) {
      __status_code_ = NOT_ALLOWED_405;
      return;
    }
    __delete_target_file();
  }
};

std::map<int, std::string> init_response_status_phrase_map();
std::map<int, std::string> init_page_contents_map();

#endif /* SRCS_HTTP_RESPONSE_RESPONSE_HPP */
