#ifndef SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP
#define SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP

#include "config/Config.hpp"
#include "event/Response.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/Path.hpp"

typedef int fileFd;

namespace response_generator {

class ResponseGenerator {
public:
  struct Content {
    enum Action {
      READ,   // FD読み込み待ち
      WRITE,  // FD書き込み待ち
      CREATED // contentを作成済み
    };
    Action      action_;
    fileFd      fd_;
    std::string str_;

    Content()
        : fd_(-1) {}
  };
  Content content_;

private:
  const RequestInfo _request_info_;
  HttpStatusCode    _status_code_;
  bool              _is_connection_close_;

public:
  ResponseGenerator(const RequestInfo &request_info,
                    HttpStatusCode     status_code = HttpStatusCode::S_200_OK);
  ~ResponseGenerator() {}
  Response        generate_response();
  bool            has_fd() const { return content_.fd_ != -1; }
  Content::Action content_state() const { return content_.action_; }
  std::string     create_response_message(const std::string &content);
  std::string     create_response_message(HttpStatusCode status_code);

private:
  static Result<std::string>
                 _read_file_to_str_cgi(const RequestInfo &request_info,
                                       const Path        &target_path);
  Content        _create_status_code_content(const RequestInfo &request_info);
  HttpStatusCode _handle_method(const RequestInfo &request_info);
  HttpStatusCode _method_get(const RequestInfo &request_info);
  HttpStatusCode _method_get_dir(const RequestInfo &request_info,
                                 const Path        &target_path);
  HttpStatusCode _method_get_file(const RequestInfo &request_info,
                                  const Path        &target_path);
  HttpStatusCode _method_post(const RequestInfo &request_info);
  static HttpStatusCode _method_delete(const RequestInfo &request_info);
};

std::string create_autoindex_body(const RequestInfo &request_info,
                                  const Path        &target_path);
std::string create_default_body_content(const HttpStatusCode &status_code);

} // namespace response_generator

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
