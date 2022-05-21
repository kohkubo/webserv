#ifndef SRCS_HTTP_REQUEST_REQUESTINFO_HPP
#define SRCS_HTTP_REQUEST_REQUESTINFO_HPP

#include <map>
#include <stdexcept>
#include <string>

#include "utils/tokenize.hpp"

enum HttpMethod { GET, POST, DELETE, UNKNOWN };

enum ContentType { URLENCODED, SOMETYPE, NONTYPE };

enum HttpStatusCode {
  NONE = -1,
  OK_200,
  NO_CONTENT_204,
  BAD_REQUEST_400,
  FORBIDDEN_403,
  NOT_FOUND_404,
  INTERNAL_SERVER_ERROR_500,
  NOT_IMPLEMENTED_501,
  UNKNOWN_ERROR_520,
};

class RequestInfo {
private:
  std::map<std::string, std::string> __field_map_;

public:
  HttpMethod               method_;
  std::string              target_;
  std::string              version_;
  std::string              host_;
  std::string              port_;
  bool                     is_close_;
  std::size_t              content_length_;
  ContentType              content_type_;
  std::vector<std::string> values_;

public:
  RequestInfo()
      : method_(UNKNOWN)
      , is_close_(false)
      , content_length_(0)
      , content_type_(NONTYPE) {}

  class BadRequestException : public std::logic_error {
  public:
    BadRequestException(const std::string &msg = "Illegal request.");
  };

  void parse_request_header(const std::string &request_header);
  void parse_request_body(const std::string &request_body);

  bool is_expected_body() const {
    // TODO: chunked
    return content_length_ != 0;
  }
  std::string &get_field_value(const std::string &field_name) {
    return __field_map_[field_name];
  }

private:
  void        __parse_request_line(const std::string &request_line);
  HttpMethod  __parse_request_method(const std::string &method);
  void        __create_header_map(tokenIterator it, tokenIterator end);
  bool        __is_comma_sparated(std::string &field_name);
  std::string __trim_optional_whitespace(std::string str);
  void        __parse_request_host();
  void        __parse_request_connection();
  void        __parse_request_content_length();
  void        __parse_request_content_type();
  void        __parse_request_values(const std::string &request_body);
};

#endif /* SRCS_HTTP_REQUEST_REQUESTINFO_HPP */
