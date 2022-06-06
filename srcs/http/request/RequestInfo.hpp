#ifndef SRCS_HTTP_REQUEST_REQUESTINFO_HPP
#define SRCS_HTTP_REQUEST_REQUESTINFO_HPP

#include <map>
#include <stdexcept>
#include <string>

#include "http/const/const_http_enums.hpp"
#include "utils/tokenize.hpp"

class RequestInfo {
private:
public:
  bool        is_blank_first_line_;
  std::string method_;
  std::string uri_; // TODO: 名前もっと適切なの考える nakamoto kohkubo
  std::string              version_;
  std::string              host_;
  bool                     connection_close_;
  bool                     is_chunked_;
  std::size_t              content_length_;
  std::string              content_type_;
  std::vector<std::string> env_values_;

private:
  static bool        __is_comma_sparated(std::string &field_name);
  static std::string __parse_request_host(const std::string &host_line);
  static bool        __parse_request_connection(const std::string &connection);
  static size_t
       __parse_request_content_length(const std::string &content_length);
  void __parse_request_values(const std::string &request_body);
  static bool
  __parse_request_transfer_encoding(const std::string &transfer_encoding);
  static std::string
  __parse_request_content_type(const std::string &content_type);
  static std::string __trim_optional_whitespace(std::string str);

public:
  RequestInfo()
      : is_blank_first_line_(false)
      , method_("")
      , connection_close_(false)
      , is_chunked_(false)
      , content_length_(0) {}

  class BadRequestException : public std::logic_error {
  private:
    HttpStatusCode __status_;

  public:
    BadRequestException(HttpStatusCode     status = BAD_REQUEST_400,
                        const std::string &msg    = "Illegal request.");
    HttpStatusCode status() const;
  };

  static void store_request_header_field_map(
      const std::string                  &header_line,
      std::map<std::string, std::string> &header_field_map);
  static void
       check_bad_parse_request_start_line(const std::string &request_line);
  void parse_request_start_line(const std::string &request_line);
  void parse_request_header(
      const std::map<std::string, std::string> &header_field_map);
  void parse_request_body(std::string       &request_body,
                          const std::string &content_type);
  void check_first_multi_blank_line(const std::string &request_line);
};

#endif /* SRCS_HTTP_REQUEST_REQUESTINFO_HPP */
