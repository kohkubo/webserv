#ifndef SRCS_HTTP_REQUEST_REQUESTINFO_HPP
#define SRCS_HTTP_REQUEST_REQUESTINFO_HPP

#include <map>
#include <stdexcept>
#include <string>

#include "config/Config.hpp"
#include "config/Location.hpp"
#include "http/const/const_http_enums.hpp"
#include "utils/tokenize.hpp"

class RequestInfo {
private:
  static const std::string OWS_;

public:
  struct ContentInfo {
    std::string                        type_;
    std::map<std::string, std::string> parameter_;
  };
  struct Form {
    ContentInfo content_disposition_;
    ContentInfo content_type_;
    std::string content_;
  };
  typedef std::map<std::string, Form> formMap;

  bool            is_blank_first_line_;
  std::string     method_;
  std::string     request_target_;
  std::string     query_string_;
  std::string     version_;
  std::string     host_;
  std::string     body_;
  bool            connection_close_;
  bool            is_chunked_;
  std::size_t     content_length_;
  std::string     target_path_;
  Config          config_;
  const Location *location_;
  ContentInfo     content_type_;
  formMap         form_map_;

private:
  static formMap     _parse_request_multi_part(const std::string &request_body,
                                               const ContentInfo &content_type);
  static formMap     _parse_multi_part_loop(std::string        body,
                                            const std::string &boudary_specified);
  static void        _parse_form_header(const std::string  line,
                                        RequestInfo::Form &form);
  static ContentInfo _parse_content_info(const std::string &content);

public:
  RequestInfo()
      : is_blank_first_line_(false)
      , method_("")
      , connection_close_(false)
      , is_chunked_(false)
      , content_length_(-1) {}

  class BadRequestException : public std::logic_error {
  private:
    HttpStatusCode::StatusCode _status_;

  public:
    BadRequestException(
        HttpStatusCode::StatusCode status = HttpStatusCode::BAD_REQUEST_400,
        const std::string         &msg    = "Illegal request.");
    HttpStatusCode::StatusCode status() const;
  };

  bool has_body() const { return content_length_ != -1 || is_chunked_; }

  static void store_request_header_field_map(
      const std::string                  &header_line,
      std::map<std::string, std::string> &header_field_map);
  static void
       check_bad_parse_request_start_line(const std::string &request_line);
  void parse_request_start_line(const std::string &request_line);
  void parse_request_header(
      const std::map<std::string, std::string> &header_field_map);
  void parse_request_body(std::string       &request_body,
                          const ContentInfo &content_type);
  void check_first_multi_blank_line(const std::string &request_line);
};

#endif /* SRCS_HTTP_REQUEST_REQUESTINFO_HPP */
