#ifndef SRCS_HTTP_RESPONSE_RESPONSECLASS_HPP
#define SRCS_HTTP_RESPONSE_RESPONSECLASS_HPP

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
  std::string __read_file_tostring_cgi(const std::string &path);

  void        __set_status_phrase();
  void        __set_general_header();
  void        __set_entity_header();
  void        __set_content_len();
  void        __set_content_type();
  void        __make_message_string();
  void        __make_bodiless_message_string();
  bool        __is_bodiless_response();

  void        __resolve_uri();
  bool        __is_error_status_code();
  bool        __is_minus_depth();
  void        __check_filepath_status();
  void        __set_error_page_contents();
  void        __set_body();

  void        __get_method_handler() { __check_filepath_status(); }
  void        __delete_target_file();
  void        __delete_method_handler() { __delete_target_file(); }
};

std::map<int, std::string> init_response_status_phrase_map();
std::map<int, std::string> init_page_contents_map();

#endif /* SRCS_HTTP_RESPONSE_RESPONSECLASS_HPP */
