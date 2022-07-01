#include "http/response/ResponseGenerator.hpp"

#include "http/const/const_delimiter.hpp"
#include "utils/Path.hpp"

namespace response_generator {

std::string create_default_body_content(const HttpStatusCode &status_code) {
  LOG("create_default_body_content");
  return "<!DOCTYPE html>\n"
         "<html>\n"
         "    <head>\n"
         "        <title>" +
         status_code.str() +
         "</title>\n"
         "    </head>\n"
         "    <body>\n"
         "<h2>" +
         status_code.status_phrase() +
         "</h2>\n"
         "provided by webserv\n"
         "    </body>\n"
         "</html>";
}

static std::string start_line(const HttpStatusCode &status_code) {
  return "HTTP/1.1" + SP + status_code.status_phrase() + CRLF;
}

static std::string connection_header() { return "Connection: close" + CRLF; }

static std::string location_header(const std::string &path) {
  return "Location: " + path + CRLF;
}

static std::string entity_header_and_body(const std::string &body) {
  return "Content-Length: " + to_string(body.size()) + CRLF +
         "Content-Type: text/html" + CRLF + CRLF + body;
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
    returnMap::const_iterator it =
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
ResponseGenerator::create_response_message(const std::string &body) {
  std::string response = create_response_header(
      _request_info_, _is_connection_close_, _status_code_);
  response += entity_header_and_body(body);
  return response;
};

std::string
ResponseGenerator::create_response_message(HttpStatusCode status_code) {
  std::string response = create_response_header(
      _request_info_, _is_connection_close_, _status_code_);
  response += entity_header_and_body(create_default_body_content(status_code));
  return response;
};

} // namespace response_generator