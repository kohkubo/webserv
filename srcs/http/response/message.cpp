#include "http/response/ResponseGenerator.hpp"

#include <string>

#include "http/const/const_delimiter.hpp"
#include "http/request/RequestInfo.hpp"

namespace response_generator {

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

static std::string create_response_header(const RequestInfo    &request_info,
                                          const HttpStatusCode &status_code) {
  // LOG("status_code: " << status_code);
  std::string response = start_line(status_code);
  if (request_info.is_close_connection() || status_code.is_connection_close()) {
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

std::string create_response_message(const RequestInfo  &request_info,
                                    const ResponseInfo &response_info,
                                    const std::string  &content) {
  std::string response =
      create_response_header(request_info, response_info.status_code_);
  response += entity_header_and_body(content);
  return response;
};

std::string create_response_message(const RequestInfo &request_info,
                                    const CgiInfo     &cgi_info) {
  std::string response = cgi_info.http_start_line();
  response += cgi_info.header_field_map_.to_string();
  if (request_info.is_close_connection() ||
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