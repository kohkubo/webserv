#include "http/response/ResponseGenerator.hpp"

#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <stdexcept>

#include "config/Config.hpp"
#include "config/Location.hpp"
#include "event/Response.hpp"
#include "http/const/const_http_enums.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

std::map<int, std::string> init_response_status_phrase_map() {
  std::map<int, std::string> res;
  res[200] = STATUS_200_PHRASE;
  res[201] = STATUS_201_PHRASE;
  res[202] = STATUS_202_PHRASE;
  res[204] = STATUS_204_PHRASE;
  res[206] = STATUS_206_PHRASE;
  res[301] = STATUS_301_PHRASE;
  res[302] = STATUS_302_PHRASE;
  res[303] = STATUS_303_PHRASE;
  res[304] = STATUS_304_PHRASE;
  res[307] = STATUS_307_PHRASE;
  res[308] = STATUS_308_PHRASE;
  res[400] = STATUS_400_PHRASE;
  res[401] = STATUS_401_PHRASE;
  res[402] = STATUS_402_PHRASE;
  res[403] = STATUS_403_PHRASE;
  res[404] = STATUS_404_PHRASE;
  res[405] = STATUS_405_PHRASE;
  res[406] = STATUS_406_PHRASE;
  res[408] = STATUS_408_PHRASE;
  res[409] = STATUS_409_PHRASE;
  res[410] = STATUS_410_PHRASE;
  res[411] = STATUS_411_PHRASE;
  res[412] = STATUS_412_PHRASE;
  res[413] = STATUS_413_PHRASE;
  res[414] = STATUS_414_PHRASE;
  res[415] = STATUS_415_PHRASE;
  res[416] = STATUS_416_PHRASE;
  res[421] = STATUS_421_PHRASE;
  res[429] = STATUS_429_PHRASE;
  res[500] = STATUS_500_PHRASE;
  res[501] = STATUS_501_PHRASE;
  res[502] = STATUS_502_PHRASE;
  res[503] = STATUS_503_PHRASE;
  res[504] = STATUS_504_PHRASE;
  res[505] = STATUS_505_PHRASE;
  res[507] = STATUS_507_PHRASE;
  res[520] = STATUS_520_PHRASE;
  return res;
}

std::map<int, std::string> g_response_status_phrase_map =
    init_response_status_phrase_map();

static bool is_error_status_code(HttpStatusCode::StatusCode status_code) {
  // TODO: エラーのステータスコードの扱いを決まったら再実装
  return 399 < status_code && status_code < 600;
}

// TODO: config.error_page validate
static ResponseGenerator::Body
body_of_status_code(const RequestInfo         &request_info,
                    HttpStatusCode::StatusCode status_code) {
  ResponseGenerator::Body      body;
  errorPageMap::const_iterator it =
      request_info.config_.error_pages_.find(status_code);
  if (it != request_info.config_.error_pages_.end() &&
      request_info.location_ != NULL) {
    std::string file_path = request_info.location_->root_ + it->second;
    int         fd        = open(file_path.c_str(), O_RDONLY);
    if (fd < 0) {
      ERROR_LOG("open error: " << file_path);
      status_code = HttpStatusCode::INTERNAL_SERVER_ERROR_500;
    } else {
      body.fd_     = fd;
      body.has_fd_ = true;
      return body;
    }
  }
  body.content_ = "<!DOCTYPE html>\n"
                  "<html>\n"
                  "    <head>\n"
                  "        <title>" +
                  to_string(status_code) +
                  "</title>\n"
                  "    </head>\n"
                  "    <body>\n"
                  "<h2>" +
                  g_response_status_phrase_map[status_code] +
                  "</h2>\n"
                  "provided by webserv\n"
                  "    </body>\n"
                  "</html>";
  return body;
}

ResponseGenerator::Body
ResponseGenerator::_create_body(const RequestInfo               &request_info,
                                const HttpStatusCode::StatusCode status_code) {
  Body body;
  if (is_error_status_code(status_code) ||
      HttpStatusCode::MOVED_PERMANENTLY_301 == status_code) {
    return body_of_status_code(request_info, status_code);
  }
  if (has_suffix(request_info.target_path_, ".py")) {
    Result<std::string> result = _read_file_to_str_cgi(request_info);
    if (result.is_err_) {
      return body_of_status_code(request_info,
                                 HttpStatusCode::INTERNAL_SERVER_ERROR_500);
    }
    body.content_ = result.object_;
    return body;
  }
  if (has_suffix(request_info.target_path_, "/")) {
    body.content_ = _create_autoindex_body(request_info);
    return body;
  }
  Result<std::string> result = read_file_to_str(request_info.target_path_);
  if (result.is_err_) {
    return body_of_status_code(request_info,
                               HttpStatusCode::INTERNAL_SERVER_ERROR_500);
  }
  body.content_ = result.object_;
  return body;
}

static std::string start_line(const HttpStatusCode::StatusCode status_code) {
  return "HTTP/1.1" + SP + g_response_status_phrase_map[status_code] + CRLF;
}

static std::string connection_header() { return "Connection: close" + CRLF; }

static std::string location_header(const returnMap           &return_map,
                                   HttpStatusCode::StatusCode status_code) {
  returnMap::const_iterator it = return_map.find(status_code);
  return "Location: " + it->second + CRLF;
}

static std::string entity_header_and_body(const std::string &body) {
  return "Content-Length: " + to_string(body.size()) + CRLF +
         "Content-Type: text/html" + CRLF + CRLF + body;
}

static std::string
create_response_header(const RequestInfo         &request_info,
                       const bool                 is_connection_close,
                       HttpStatusCode::StatusCode status_code) {
  // LOG("status_code: " << status_code);
  std::string response = start_line(status_code);
  if (is_connection_close) {
    response += connection_header();
  }
  if (HttpStatusCode::NO_CONTENT_204 == status_code) {
    return response + CRLF;
  }
  if (HttpStatusCode::MOVED_PERMANENTLY_301 == status_code) {
    response +=
        location_header(request_info.location_->return_map_, status_code);
  }
  return response;
}

static std::string create_response_message(
    const RequestInfo &request_info, const bool is_connection_close,
    HttpStatusCode::StatusCode status_code, const std::string &body) {
  std::string response =
      create_response_header(request_info, is_connection_close, status_code);
  response += entity_header_and_body(body);
  return response;
};

ResponseGenerator::ResponseInfo ResponseGenerator::_get_statuscode_body(
    const RequestInfo &request_info, HttpStatusCode::StatusCode status_code) {
  if (request_info.location_ == NULL) {
    status_code = HttpStatusCode::NOT_FOUND_404;
    return ResponseInfo(status_code,
                        body_of_status_code(request_info, status_code));
  }
  if (is_error_status_code(status_code)) {
    // LOG("status_code: " << status_code);
    return ResponseInfo(status_code,
                        body_of_status_code(request_info, status_code));
  }
  if (request_info.location_->return_map_.size() != 0) {
    status_code = static_cast<HttpStatusCode::StatusCode>(
        request_info.location_->return_map_.begin()->first);
    return ResponseInfo(status_code,
                        body_of_status_code(request_info, status_code));
  }
  status_code = _handle_method(request_info);
  return ResponseInfo(status_code, _create_body(request_info, status_code));
}

Response
ResponseGenerator::generate_response(const RequestInfo &request_info,
                                     const bool         is_connection_close,
                                     HttpStatusCode::StatusCode status_code) {
  ResponseInfo response_info = _get_statuscode_body(request_info, status_code);
  Body         body          = response_info.body_;
  status_code                = response_info.status_code_;
  if (body.has_fd_) {
    Result<std::string> result = read_fd(body.fd_);
    if (result.is_err_) {
      status_code = HttpStatusCode::INTERNAL_SERVER_ERROR_500;
      body        = body_of_status_code(request_info, status_code);
    } else {
      body.content_ = result.object_;
    }
  }
  std::string response = create_response_message(
      request_info, is_connection_close, status_code, body.content_);
  return Response(response, is_connection_close);
}
