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
  ResponseInfo       response_info_;
  const RequestInfo  request_info_;
  const std::string &peer_name_;
  size_t             redirect_count_; // response infoの中に入る

private:
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
  // レスポンスinfoのコンストラクタにする
  ResponseGenerator(const RequestInfo &request_info,
                    const std::string &peer_name, HttpStatusCode status_code,
                    size_t redirect_count = 0);
  ~ResponseGenerator() {}
  Response generate_response(
      const HttpStatusCode &status_code) const; // response_infoに移動
  bool is_over_max_redirect_count() const {
    return redirect_count_ > MAX_REDIRECT_COUNT_;
  }
  bool need_socket() const {
    return response_info_.action_ != ResponseInfo::CREATED;
  }
  ns_socket::SocketBase *create_socket(Response                &response,
                                       ns_socket::ClientSocket *parent_socket);
};

Result<ResponseInfo> create_cgi_content(const RequestInfo &request_info,
                                        const std::string &peer_name,
                                        const Path        &target_path);
std::string          create_autoindex_body(const RequestInfo &request_info,
                                           const Path        &target_path);

std::string create_response_message(const RequestInfo  &request_info,
                                    const ResponseInfo &response_info,
                                    const std::string  &content);
std::string create_response_message(const RequestInfo &request_info,
                                    const CgiInfo     &cgi_info);

} // namespace response_generator

#endif /* SRCS_HTTP_RESPONSE_RESPONSEGENERATOR_HPP */
