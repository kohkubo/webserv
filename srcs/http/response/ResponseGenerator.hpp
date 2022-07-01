#ifndef SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP
#define SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP

#include "config/Config.hpp"
#include "event/Response.hpp"
#include "http/request/RequestInfo.hpp"

typedef int fileFd;

namespace response_generator {

class ResponseGenerator {
public:
  struct ContentAction {
    enum Action { READ, WRITE, CREATED };
    Action      action_;
    fileFd      fd_;
    std::string content_;

    ContentAction()
        : fd_(-1) {}
    bool has_fd() const { return fd_ != -1; }
  };
  ContentAction content_action_;

private:
  const RequestInfo _request_info_;
  HttpStatusCode    _status_code_;
  bool              _is_connection_close_;

public:
  ResponseGenerator(const RequestInfo &request_info,
                    HttpStatusCode     status_code = HttpStatusCode::S_200_OK);
  ~ResponseGenerator() {}
  Response    generate_response();
  bool        has_fd() const { return content_action_.has_fd(); }
  int         fd() const { return content_action_.fd_; }
  std::string create_response_message(const std::string &body);
  std::string create_response_message(HttpStatusCode status_code);

private:
  static Result<std::string>
                 _read_file_to_str_cgi(const RequestInfo &request_info,
                                       const std::string &target_path);
  ContentAction  _create_status_code_body(const RequestInfo &request_info);
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
