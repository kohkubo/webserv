#include "http/response/ResponseGenerator.hpp"

#include <cstdlib>
#include <unistd.h>

#include <iostream>
#include <map>
#include <stdexcept>

#include "config/Config.hpp"
#include "config/Location.hpp"
#include "event/Response.hpp"
#include "http/HttpStatusCode.hpp"
#include "http/const/const_delimiter.hpp"
#include "http/request/RequestInfo.hpp"
#include "socket/CgiSocket.hpp"
#include "socket/FileReadSocket.hpp"
#include "socket/FileWriteSocket.hpp"
#include "socket/SocketBase.hpp"
#include "utils/utils.hpp"

namespace response_generator {
ResponseGenerator::ResponseGenerator(const RequestInfo &request_info,
                                     const std::string &peer_name,
                                     HttpStatusCode     status_code)
    : request_info_(request_info)
    , peer_name_(peer_name)
    , _redirect_count_(0) {
  if (request_info_.config_ == NULL) {
    response_info_ =
        ResponseInfo(ResponseInfo::CREATED,
                     status_code.create_default_content_str(), status_code);
    return;
  }
  if (status_code.is_error_status_code()) {
    response_info_ = _create_status_code_content(status_code);
    return;
  }
  if (request_info_.transfer_encoding_.size() != 0 &&
      request_info_.is_chunked()) {
    response_info_ =
        _create_status_code_content(HttpStatusCode::S_501_NOT_IMPLEMENTED);
    return;
  }
  if (request_info_.location_ == NULL) {
    response_info_ =
        _create_status_code_content(HttpStatusCode::S_404_NOT_FOUND);
    return;
  }
  if (request_info_.location_->return_map_.size() != 0) {
    status_code    = request_info_.location_->return_map_.begin()->first;
    response_info_ = _create_status_code_content(status_code);
    return;
  }
  Path target_path = request_info_.location_->root_ +
                     request_info_.request_line_.absolute_path_;
  response_info_ = _handle_method(target_path);
}

// reqponse_info_に移動
Response
ResponseGenerator::generate_response(const HttpStatusCode &status_code) const {
  bool is_connection_close =
      status_code.is_connection_close() || request_info_.is_close_connection();
  switch (response_info_.action_) {
  case ResponseInfo::READ:
    return Response(is_connection_close, Response::READING);
  case ResponseInfo::WRITE:
    return Response(is_connection_close, Response::WRITING);
  case ResponseInfo::CGI:
    return Response(is_connection_close, Response::READING);
  default:
    return Response(create_response_message(request_info_, response_info_,
                                            response_info_.content_),
                    is_connection_close);
  }
}

// actionを引数で渡すようにして外に出す
ns_socket::SocketBase *
ResponseGenerator::create_socket(Response                &response,
                                 ns_socket::ClientSocket *parent_socket) {
  switch (response_info_.action_) {
  case ResponseInfo::READ:
    return new ns_socket::FileReadSocket(response, *this, parent_socket);
  case ResponseInfo::WRITE:
    return new ns_socket::FileWriteSocket(response, *this, parent_socket);
  case ResponseInfo::CGI:
    return new ns_socket::CgiSocket(response, *this, parent_socket);
  default:
    return NULL;
  }
}

ResponseGenerator ResponseGenerator::create_response_generator(
    const HttpStatusCode &status_code) const {
  return ResponseGenerator(request_info_, peer_name_, status_code);
}

ResponseGenerator
ResponseGenerator::create_response_generator(const std::string &target_path,
                                             const std::string &query) const {
  RequestInfo request_info                  = request_info_;
  request_info.request_line_.absolute_path_ = target_path;
  request_info.request_line_.query_         = query;

  ResponseGenerator response_generator(request_info, peer_name_);
  response_generator._redirect_count_ = _redirect_count_ + 1;

  return response_generator;
}

} // namespace response_generator