#include "http/response/ResponseGenerator.hpp"

#include <cstdlib>
#include <unistd.h>

#include <iostream>
#include <map>
#include <stdexcept>

#include "config/Config.hpp"
#include "config/Location.hpp"
#include "http/HttpStatusCode.hpp"
#include "http/const/const_delimiter.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/Response.hpp"
#include "socket/CgiSocket.hpp"
#include "socket/FileReadSocket.hpp"
#include "socket/FileWriteSocket.hpp"
#include "socket/SocketBase.hpp"
#include "utils/utils.hpp"

namespace response_generator {
ResponseGenerator::ResponseGenerator(const RequestInfo &request_info,
                                     const std::string &peer_name,
                                     HttpStatusCode     status_code,
                                     size_t             redirect_count)
    : request_info_(request_info)
    , peer_name_(peer_name)
    , redirect_count_(redirect_count) {
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
Response ResponseGenerator::generate_response() const {
  bool is_connection_close =
      response_info_.status_code_.is_connection_close() ||
      request_info_.is_close_connection();
  switch (response_info_.action_) {
  case ResponseInfo::READ:
    return Response(Response::READING, is_connection_close);
  case ResponseInfo::WRITE:
    return Response(Response::WRITING, is_connection_close);
  case ResponseInfo::CGI:
    return Response(Response::READING, is_connection_close);
  default:
    return Response(Response::SENDING, is_connection_close,
                    create_response_message(request_info_, response_info_,
                                            response_info_.content_));
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

} // namespace response_generator