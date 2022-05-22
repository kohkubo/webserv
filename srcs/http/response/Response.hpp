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
  std::string        __version_;
  std::string        __host_;
  std::string        __content_len_;
  std::string        __content_type_;
  std::string        __connection_;
  std::string        __body_;

public:
  Response(const Config &config, const RequestInfo &request_info);
  virtual ~Response(){};
  std::string get_response_string() {
    __set_response_body();
    __make_message_string();
    return __response_string_;
  }

private:
  Response();
  std::string __read_file_tostring_cgi(const std::string &path);

  // clang-format off
  void __set_response_body() {
    if (__status_code_ == NO_CONTENT_204 || __status_code_ == NOT_MODIFIED_304) {
      return;
    }
    __set_content_len();
    __set_content_type();
  }
  // clang-format on
  void __set_content_len() { __content_len_ = to_string(__body_.size()); };
  void __set_content_type() { __content_type_ = TEXT_HTML; };
  void __make_message_string() {
    // start line
    __response_string_ = __version_ + SP + __status_phrase_ + CRLF;
    // header
    if (!(__status_code_ == NO_CONTENT_204 ||
          __status_code_ == NOT_MODIFIED_304)) {
      __response_string_ += "Content-Length: " + __content_len_ + CRLF;
      __response_string_ += "Content-Type: " + __content_type_ + CRLF;
    }
    __response_string_ += "Connection: " + __connection_ + CRLF;
    // empty line
    __response_string_ += CRLF;
    if (__status_code_ == NO_CONTENT_204 ||
        __status_code_ == NOT_MODIFIED_304) {
      return;
    }
    // body
    __response_string_ += __body_;
  };

  void __resolve_uri();
  bool __is_minus_depth();
  void __check_filepath_status();
  void __set_error_page_body();
  void __set_body();

  void __get_method_handler() {}
  void __delete_target_file();
  void __delete_method_handler() { __delete_target_file(); }
};

std::map<int, std::string> init_response_status_phrase_map();
std::map<int, std::string> init_page_contents_map();

#endif /* SRCS_HTTP_RESPONSE_RESPONSECLASS_HPP */
