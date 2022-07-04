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
      READ,    // FD読み込み待ち
      WRITE,   // FD書き込み待ち
      CREATED, // contentを作成済み
      CGI      // CGI
    };
    Action         action_;
    HttpStatusCode status_code_;
    std::string    str_;
    fileFd         fd_;
    pid_t          cgi_pid_;

    Content(HttpStatusCode status_code = HttpStatusCode::S_200_OK)
        : action_(READ)
        , status_code_(status_code)
        , fd_(-1) {}
  };
  Content content_;

private:
  const RequestInfo _request_info_;
  bool              _is_connection_close_;

public:
  ResponseGenerator(const RequestInfo &request_info,
                    HttpStatusCode     status_code = HttpStatusCode::S_200_OK);
  ~ResponseGenerator() {}
  Response        generate_response();
  Content::Action action() const { return content_.action_; }
  std::string     create_response_message(const std::string &content);
};

std::string create_default_body_content(const HttpStatusCode &status_code);
Result<ResponseGenerator::Content>
read_file_to_str_cgi(const RequestInfo &request_info, const Path &target_path);
std::string create_autoindex_body(const RequestInfo &request_info,
                                  const Path        &target_path);
ResponseGenerator::Content
create_status_code_content(const RequestInfo    &request_info,
                           const HttpStatusCode &status_code);
ResponseGenerator::Content handle_method(const RequestInfo &request_info);
} // namespace response_generator

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
