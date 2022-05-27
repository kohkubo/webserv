#ifndef SRCS_HTTP_REQUEST_REQUESTINFO_HPP
#define SRCS_HTTP_REQUEST_REQUESTINFO_HPP

#include <map>
#include <stdexcept>
#include <string>

#include "http/const/const_http_enums.hpp"
#include "utils/tokenize.hpp"

class RequestInfo {
private:
  bool                               __is_first_time_;
  std::map<std::string, std::string> __field_map_;

public:
  HttpMethod method_;
  std::string uri_; // TODO: 名前もっと適切なの考える nakamoto kohkubo
  std::string              version_;
  std::string              host_;
  std::string              port_;
  bool                     is_close_;
  std::size_t              content_length_;
  std::vector<std::string> values_;

private:
  std::string __cut_buffer(std::string &buf, std::size_t len);
  void        __parse_request_line(const std::string &request_line);
  HttpMethod  __request_method_to_enum(const std::string &method);
  bool        __is_comma_sparated(std::string &field_name);
  std::string __trim_optional_whitespace(std::string str);
  void        __parse_request_host();
  void        __parse_request_connection();
  void        __parse_request_content_length();
  void        __parse_request_values(const std::string &request_body);

public:
  RequestInfo()
      : __is_first_time_(true)
      , method_(UNKNOWN)
      , is_close_(false)
      , content_length_(0) {}

  class BadRequestException : public std::logic_error {
  public:
    BadRequestException(const std::string &msg = "Illegal request.");
  };

  void        parse_request_header_field(const std::string &header_line);
  void        parse_request_start_line(const std::string &request_line);
  void        parse_request_header();
  bool        has_request_body(const std::string &request_buffer);
  std::string cut_request_body(std::string &request_buffer);
  void        parse_request_body(std::string &request_body);
  void        check_first_mulit_blank_line(const std::string &request_line);
  void check_bad_parse_request_start_line(const std::string &request_line);
};

#endif /* SRCS_HTTP_REQUEST_REQUESTINFO_HPP */
