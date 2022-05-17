#ifndef SRCS_HTTP_REQUEST_REQUESTINFO_HPP
#define SRCS_HTTP_REQUEST_REQUESTINFO_HPP

#include <map>
#include <string>

#include "utils/tokenize.hpp"

enum HttpMethod { GET, POST, DELETE, UNKNOWN };
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
  HttpMethod                         __method_;
  std::string                        __target_;
  std::string                        __version_;
  std::map<std::string, std::string> __field_map_;
  std::string                        __host_;
  int                                __port_;

public:
  RequestInfo()
      : __method_(UNKNOWN) {}

  class BadRequestException : public std::logic_error {
  public:
    BadRequestException(const std::string &msg = "Illegal request.");
  };

  void parse_request_header(const std::string &request_string);

private:
  void        __parse_request_line(const std::string &request_line);
  HttpMethod  __parse_request_method(const std::string &method);
  void        __create_header_map(token_iterator it, token_iterator end);
  bool        __is_comma_sparated(std::string &field_name);
  std::string __trim_space(std::string str);
  void        __parse_request_host();
};

#endif /* SRCS_HTTP_REQUEST_REQUESTINFO_HPP */
