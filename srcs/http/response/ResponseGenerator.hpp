#ifndef SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP
#define SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP

#include "config/Config.hpp"
#include "config/Location.hpp"
#include "event/Response.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/CgiInfo.hpp"
#include "http/response/ResponseInfo.hpp"
#include "socket/ClientSocket.hpp"
#include "socket/SocketBase.hpp"
#include "utils/Path.hpp"

namespace response_generator {

class ResponseGenerator {
public:
  ResponseInfo            response_info_;
  const RequestInfo       request_info_;
  const std::string      &peer_name_;
  const config::Location *location_;

private:
  bool                _is_connection_close_;
  size_t              _redirect_count_;
  static const size_t MAX_REDIRECT_COUNT_ = 10;

private:
  ResponseInfo _handle_method(const Path &target_path);
  ResponseInfo _method_get(const Path &target_path);
  ResponseInfo _method_get_file(const Path &target_path);
  ResponseInfo _method_get_dir(const Path &target_path);
  ResponseInfo _method_post(const Path &target_path);
  ResponseInfo _method_delete(const Path &target_path);
  ResponseInfo _method_cgi(const Path &target_path);
  ResponseInfo _create_status_code_content(const HttpStatusCode &status_code);

public:
  ResponseGenerator(const RequestInfo &request_info,
                    const std::string &peer_name,
                    HttpStatusCode     status_code = HttpStatusCode::S_200_OK);
  ~ResponseGenerator() {}
  Response generate_response() const;
  bool     is_over_max_redirect_count() const {
        return _redirect_count_ > MAX_REDIRECT_COUNT_;
  }
  bool need_socket() const {
    return response_info_.action_ != ResponseInfo::CREATED;
  }
  ns_socket::SocketBase *create_socket(Response                &response,
                                       ns_socket::ClientSocket *parent_socket);
  std::string create_response_message(const std::string &content) const;
  std::string create_response_message(const CgiInfo &cgi_info) const;
  ResponseGenerator
  new_status_response_generator(const HttpStatusCode &new_status_code) const;
  ResponseGenerator
  new_target_response_generator(const std::string &new_target,
                                const std::string &new_query) const;
};

Result<ResponseInfo> create_cgi_content(const RequestInfo &request_info,
                                        const std::string &peer_name,
                                        const Path        &target_path);
std::string          create_autoindex_body(const RequestInfo &request_info,
                                           const Path        &target_path);
} // namespace response_generator

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
