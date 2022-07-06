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
#include "utils/utils.hpp"

namespace response_generator {

ResponseGenerator::ResponseGenerator(const RequestInfo &request_info,
                                     HttpStatusCode     status_code)
    : request_info_(request_info)
    , location_(NULL)
    , _is_connection_close_(request_info_.connection_close_ ||
                            status_code == HttpStatusCode::C_400_BAD_REQUEST ||
                            status_code ==
                                HttpStatusCode::S_413_ENTITY_TOO_LARGE) {
  if (status_code.is_error_status_code()) {
    content_ = create_status_code_content(request_info_, status_code);
    return;
  }
  location_ = request_info_.config_.locations_.select_location(
      request_info_.request_line_.absolute_path_);
  if (location_ == NULL) {
    content_ = create_status_code_content(request_info_,
                                          HttpStatusCode::S_404_NOT_FOUND);
    return;
  }
  if (request_info_.location_->return_map_.size() != 0) {
    status_code = request_info_.location_->return_map_.begin()->first;
    content_    = create_status_code_content(request_info_, status_code);
    return;
  }
  content_ = handle_method(request_info_);
}

Response ResponseGenerator::generate_response() {
  switch (content_.action_) {
  case Content::READ:
    return Response(_is_connection_close_, Response::READING);
  case Content::WRITE:
    return Response(_is_connection_close_, Response::WRITING);
  case Content::CGI:
    return Response(_is_connection_close_, Response::READING);
  default:
    return Response(create_response_message(content_.str_),
                    _is_connection_close_);
  }
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

static std::string create_response_header(const RequestInfo &request_info,
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
        request_info.location_->return_map_.find(status_code);
    response += location_header(it->second);
  }
  if (HttpStatusCode::S_201_CREATED == status_code) {
    response +=
        location_header(request_info.request_line_.absolute_path_.str());
  }
  return response;
}

std::string
ResponseGenerator::create_response_message(const std::string &content) {
  std::string response = create_response_header(
      request_info_, _is_connection_close_, content_.status_code_);
  response += entity_header_and_body(content);
  return response;
};

} // namespace response_generator