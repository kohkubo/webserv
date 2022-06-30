#ifndef SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP
#define SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP

#include "config/Config.hpp"
#include "event/Response.hpp"
#include "http/request/RequestInfo.hpp"

typedef int fileFd;

namespace response_generator {

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
  const RequestInfo _request_info_;
  HttpStatusCode    _status_code_;
  bool              _is_connection_close_;
  Body              _body_;

public:
  ResponseGenerator(const RequestInfo &request_info,
                    HttpStatusCode     status_code = HttpStatusCode::S_200_OK);
  ~ResponseGenerator() {}
  Response    generate_response();
  bool        has_fd() const { return _body_.has_fd(); }
  int         fd() const { return _body_.fd_; }
  std::string create_response_message(const std::string &body);
  std::string create_response_message(HttpStatusCode status_code);

private:
  static Result<std::string>
                 _read_file_to_str_cgi(const RequestInfo &request_info,
                                       const std::string &target_path);
  Body           _create_status_code_body(const RequestInfo &request_info);
  HttpStatusCode _handle_method(const RequestInfo &request_info);
  HttpStatusCode _method_get(const RequestInfo &request_info);
  HttpStatusCode _method_get_dir(const RequestInfo &request_info,
                                 const std::string &target_path);
  HttpStatusCode _method_get_file(const RequestInfo &request_info,
                                  const std::string &target_path);
  HttpStatusCode _method_post(const RequestInfo &request_info);
  static HttpStatusCode _method_delete(const RequestInfo &request_info);
};

std::string create_autoindex_body(const RequestInfo &request_info,
                                  const std::string &target_path);
std::string create_default_body_content(const HttpStatusCode &status_code);

} // namespace response_generator

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
