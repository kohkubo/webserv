#ifndef SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP
#define SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP

#include "config/Config.hpp"
#include "event/Response.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/Content.hpp"
#include "utils/Path.hpp"

namespace response_generator {

class ResponseGenerator {
public:
  Content           content_;
  const RequestInfo request_info_;

private:
  bool _is_connection_close_;

public:
  ResponseGenerator(const RequestInfo &request_info,
                    HttpStatusCode     status_code = HttpStatusCode::S_200_OK);
  ~ResponseGenerator() {}
  Response        generate_response();
  Content::Action action() const { return content_.action_; }
  std::string     create_response_message(const std::string &content);
};

std::string     create_default_content_str(const HttpStatusCode &status_code);
Result<Content> create_cgi_content(const RequestInfo &request_info,
                                   const Path        &target_path);
std::string     create_autoindex_body(const RequestInfo &request_info,
                                      const Path        &target_path);
Content         create_status_code_content(const RequestInfo    &request_info,
                                           const HttpStatusCode &status_code);
Content         handle_method(const RequestInfo &request_info);
} // namespace response_generator

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
