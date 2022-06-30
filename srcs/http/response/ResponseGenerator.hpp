#ifndef SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP
#define SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP

#include "config/Config.hpp"
#include "event/Response.hpp"
#include "http/request/RequestInfo.hpp"

typedef int fileFd;

class ResponseGenerator {
public:
  struct Body {
    enum Action { READ, WRITE };
    Action      action_;
    fileFd      fd_;
    bool        has_content_;
    std::string content_;

    Body()
        : fd_(-1)
        , has_content_(false) {}
    bool has_fd() const { return fd_ != -1; }
  };

private:
  const RequestInfo &_request_info_;
  HttpStatusCode     _status_code_;
  bool               _is_connection_close_;
  Body               _body_;

public:
  ResponseGenerator(const RequestInfo &request_info,
                    HttpStatusCode     status_code = HttpStatusCode::S_200_OK);
  ~ResponseGenerator() {}
  Response generate_response();

private:
  static std::string _create_default_body_content(HttpStatusCode status_code);
  static Result<std::string>
                        _read_file_to_str_cgi(const RequestInfo &request_info,
                                              const std::string &target_path);
  static Body           _create_body(const RequestInfo   &request_info,
                                     const HttpStatusCode status_code);
  static std::string    _create_autoindex_body(const RequestInfo &request_info,
                                               const std::string &target_path);
  static HttpStatusCode _get_status_code(const RequestInfo &request_info,
                                         HttpStatusCode     status_code);
  static std::string _create_response_message(const RequestInfo &request_info,
                                              const bool is_connection_close,
                                              HttpStatusCode     status_code,
                                              const std::string &body);
  Body               _create_status_code_body(const RequestInfo &request_info);
  void               _handle_method(const RequestInfo &request_info);
  void _method_get(const RequestInfo &request_info, std::string target_path);
  void _method_post(const RequestInfo &request_info, std::string target_path);
  void _method_delete(const std::string &target_path);
};

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
