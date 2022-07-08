#ifndef SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP
#define SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP

#include "config/Config.hpp"
#include "config/Location.hpp"
#include "event/Response.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/ResponseInfo.hpp"
#include "utils/Path.hpp"

namespace response_generator {

class ResponseGenerator {
public:
  ResponseInfo            response_info_;
  const RequestInfo       request_info_;
  const config::Location *location_;

private:
  bool _is_connection_close_;

private:
  ResponseInfo _handle_method(const Path &target_path);
  ResponseInfo _method_get(const Path &target_path);
  ResponseInfo _method_get_file(const Path &target_path);
  ResponseInfo _method_get_dir(const Path &target_path);
  ResponseInfo _method_post(const Path &target_path);
  ResponseInfo _method_delete(const Path &target_path);
  ResponseInfo _create_status_code_content(const HttpStatusCode &status_code);

public:
  ResponseGenerator(const RequestInfo &request_info,
                    HttpStatusCode     status_code = HttpStatusCode::S_200_OK);
  ~ResponseGenerator() {}
  Response             generate_response();
  ResponseInfo::Action action() const { return response_info_.action_; }
  std::string          create_response_message(const std::string &content);
  void                 update_content(const HttpStatusCode &status_code);
};

Result<ResponseInfo> create_cgi_content(const RequestInfo &request_info,
                                        const Path        &target_path);
std::string          create_autoindex_body(const RequestInfo &request_info,
                                           const Path        &target_path);
} // namespace response_generator

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
