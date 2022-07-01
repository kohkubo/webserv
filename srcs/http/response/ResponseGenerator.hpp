#ifndef SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP
#define SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP

#include "config/Config.hpp"
#include "event/Response.hpp"
#include "http/request/RequestInfo.hpp"

typedef int fileFd;

namespace response_generator {

class ResponseGenerator {
public:
  struct Content {
    enum ContentState {
      READ,   // FD読み込み待ち
      WRITE,  // FD書き込み待ち
      CREATED // contentを作成済み
    };
    ContentState   state_;
    HttpStatusCode status_code_;
    std::string    str_;
    fileFd         fd_;

    Content(HttpStatusCode status_code = HttpStatusCode::S_200_OK)
        : state_(READ)
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
  Response              generate_response();
  Content::ContentState content_state() const { return content_.state_; }
  std::string           create_response_message(const std::string &content);
};

Result<std::string> read_file_to_str_cgi(const RequestInfo &request_info,
                                         const std::string &target_path);
std::string         create_autoindex_body(const RequestInfo &request_info,
                                          const std::string &target_path);
std::string create_default_body_content(const HttpStatusCode &status_code);
ResponseGenerator::Content
create_status_code_content(const RequestInfo    &request_info,
                           const HttpStatusCode &status_code);
ResponseGenerator::Content handle_method(const RequestInfo &request_info);
ResponseGenerator::Content method_post(const RequestInfo &request_info);
ResponseGenerator::Content method_get(const RequestInfo &request_info);
ResponseGenerator::Content method_delete(const RequestInfo &request_info);
ResponseGenerator::Content method_get_file(const RequestInfo &request_info,
                                           const std::string &target_path);
ResponseGenerator::Content method_get_dir(const RequestInfo &request_info,
                                          const std::string &target_path);
} // namespace response_generator

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
