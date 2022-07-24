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
    , _is_connection_close_(request_info_.is_close_connection() ||
                            status_code == HttpStatusCode::S_400_BAD_REQUEST ||
                            status_code ==
                                HttpStatusCode::S_413_ENTITY_TOO_LARGE)
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

Response ResponseGenerator::generate_response() const {
  switch (response_info_.action_) {
  case ResponseInfo::READ:
    return Response(_is_connection_close_, Response::READING);
  case ResponseInfo::WRITE:
    return Response(_is_connection_close_, Response::WRITING);
  case ResponseInfo::CGI:
    return Response(_is_connection_close_, Response::READING);
  default:
    return Response(create_response_message(response_info_.content_),
                    _is_connection_close_);
  }
}

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

static std::string start_line(const HttpStatusCode &status_code) {
  return "HTTP/1.1" + SP + status_code.status_phrase() + CRLF;
}

static std::string connection_header() { return "Connection: close" + CRLF; }

static std::string location_header(const std::string &path) {
  return "Location: " + path + CRLF;
}

static std::string entity_header_and_body(const std::string &content) {
  return "Content-Length: " + to_string(content.size()) + CRLF +
         "Content-Type: text/html" + CRLF + CRLF + content;
}

static std::string create_response_header(const RequestInfo      &request_info,
                                          const config::Location *location,
                                          const bool is_connection_close,
                                          const HttpStatusCode &status_code) {
  // LOG("status_code: " << status_code);
  std::string response = start_line(status_code);
  if (is_connection_close) {
    response += connection_header();
  }
  if (HttpStatusCode::S_204_NO_CONTENT == status_code) {
    return response + CRLF;
  }
  if (HttpStatusCode::S_301_MOVED_PERMANENTLY == status_code) {
    config::returnMap::const_iterator it =
        location->return_map_.find(status_code);
    response += location_header(it->second);
  }
  if (HttpStatusCode::S_201_CREATED == status_code) {
    response +=
        location_header(request_info.request_line_.absolute_path_.str());
  }
  return response;
}

std::string
ResponseGenerator::create_response_message(const std::string &content) const {
  std::string response = create_response_header(
      request_info_, request_info_.location_, _is_connection_close_,
      response_info_.status_code_);
  response += entity_header_and_body(content);
  return response;
};

std::string
ResponseGenerator::create_response_message(const CgiInfo &cgi_info) const {
  std::string response = cgi_info.http_start_line();
  response += cgi_info.header_field_map_.to_string();
  if (request_info_.is_close_connection() ||
      (!cgi_info.http_status_phrase_.empty() &&
       (400 <= cgi_info.status_digit_ && cgi_info.status_digit_ <= 499))) {
    response += connection_header();
  }
  if (!cgi_info.http_status_phrase_.empty() &&
      ((100 <= cgi_info.status_digit_ && cgi_info.status_digit_ <= 199) ||
       (cgi_info.status_digit_ == 304 || cgi_info.status_digit_ == 204))) {
    response += CRLF;
    return response;
  }
  response += "Content-Length: " + to_string(cgi_info.content_.size()) + CRLF;
  response += CRLF;
  response += cgi_info.content_;
  return response;
};

} // namespace response_generator