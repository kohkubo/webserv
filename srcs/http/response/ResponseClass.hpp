#ifndef SRCS_HTTP_METHOD_RESPONSECLASS_HPP
#define SRCS_HTTP_METHOD_RESPONSECLASS_HPP

#include "config/Config.hpp"
#include "http/const/const_delimiter.hpp"
#include "http/const/const_html_filename.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/utils.hpp"

class ResponseClass {
private:
  const Config      &__config_;
  const RequestInfo &__request_info_;
  int                __status_code_;
  std::string        __response_string_;
  std::string        __file_path_;

public:
  std::string path_;
  std::string status_phrase_;
  std::string version_;
  std::string host_;
  std::string content_len_;
  std::string content_type_;
  std::string connection_;
  std::string body_;

public:
  ResponseClass(const Config &config, const RequestInfo &request_info);
  virtual ~ResponseClass(){};
  std::string get_response_string() {
    __set_response_body();
    __make_message_string();
    return __response_string_;
  }

private:
  ResponseClass();
  std::string  __read_file_tostring_cgi(const std::string &path);

  // clang-format off
  void __set_response_body() {
    if (__status_code_ == 204 || __status_code_ == 304) {
      return;
    }
    __set_content_len();
    __set_content_type();
  }
  // clan-format on
  void __set_content_len() { content_len_ = to_string(body_.size()); };
  void __set_content_type() { content_type_ = TEXT_HTML; };
  void __make_message_string() {
    // start line
    __response_string_ = version_ + SP + status_phrase_ + CRLF;
    // header
    __response_string_ += "Host: " + host_ + CRLF;
    __response_string_ += "Content-Length: " + content_len_ + CRLF;
    __response_string_ += "Content-Type: " + content_type_ + CRLF;
    __response_string_ += "Connection: " + connection_ + CRLF;
    // empty line
    __response_string_ += CRLF;
    // body
    __response_string_ += body_;
  };

  void __resolve_url();
  bool __is_minus_depth();
  void __check_status();
  void __set_error_page_file_path();
  void __set_body();

  void __get_method_handler() {}
  void __delete_target_file();
  void __delete_method_handler() {
    __delete_target_file();
  }
};

#endif /* SRCS_HTTP_METHOD_RESPONSECLASS_HPP */
