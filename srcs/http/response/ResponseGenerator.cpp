#include "http/response/ResponseGenerator.hpp"

#include <cstdlib>
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
static std::string body_of_status_code(const RequestInfo         &request_info,
                                       HttpStatusCode::StatusCode status_code) {
  errorPageMap::const_iterator it =
      request_info.config_.error_pages_.find(status_code);
  if (it != request_info.config_.error_pages_.end() &&
      request_info.location_ != NULL) {
    std::string file_path      = request_info.location_->root_ + it->second;
    // TODO:
    // 無いエラーページを指定した場合、nginxではどのような挙動になるのかチェックが必要
    Result<std::string> result = read_file_to_str(file_path);
    if (!result.is_err_) {
      return result.object_;
    }
    ERROR_LOG("body_of_status_code: read_file_to_str error");
    status_code = HttpStatusCode::INTERNAL_SERVER_ERROR_500;
  }
  return "<!DOCTYPE html>\n"
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
}

std::string
<<<<<<< HEAD
ResponseGenerator::_create_body(const RequestInfo               &request_info,
                                const HttpStatusCode::StatusCode status_code,
                                const std::string               &target_path) {
=======
ResponseGenerator::_body(const RequestInfo               &request_info,
                         const HttpStatusCode::StatusCode status_code,
                         const std::string               &target_path) {
>>>>>>> 6deebcff37bd2787277d6d3292c6961fc9815cd4
  if (is_error_status_code(status_code) ||
      HttpStatusCode::MOVED_PERMANENTLY_301 == status_code) {
    return body_of_status_code(request_info, status_code);
  }
  if (has_suffix(target_path, ".py")) {
    Result<std::string> result =
        _read_file_to_str_cgi(request_info, target_path);
    if (result.is_err_) {
      return body_of_status_code(request_info,
                                 HttpStatusCode::INTERNAL_SERVER_ERROR_500);
    }
    return result.object_;
  }
  if (has_suffix(target_path, "/")) {
    return _create_autoindex_body(request_info, target_path);
  }
  Result<std::string> result = read_file_to_str(target_path);
  if (result.is_err_) {
    return body_of_status_code(request_info,
                               HttpStatusCode::INTERNAL_SERVER_ERROR_500);
  }
  return result.object_;
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

<<<<<<< HEAD
static std::string
create_response_header(const RequestInfo         &request_info,
                       const bool                 is_connection_close,
                       HttpStatusCode::StatusCode status_code) {
=======
std::string ResponseGenerator::response_message(
    const RequestInfo &request_info, const bool is_connection_close,
    const HttpStatusCode::StatusCode status_code,
    const std::string               &target_path) {
>>>>>>> 6deebcff37bd2787277d6d3292c6961fc9815cd4
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
<<<<<<< HEAD
  return response;
}

static std::string create_response_message(
    const RequestInfo &request_info, const bool is_connection_close,
    const HttpStatusCode::StatusCode status_code, const std::string &body) {
  std::string response =
      create_response_header(request_info, is_connection_close, status_code);
  response += entity_header_and_body(body);
=======
  response +=
      entity_header_and_body(_body(request_info, status_code, target_path));
>>>>>>> 6deebcff37bd2787277d6d3292c6961fc9815cd4
  return response;
};

Response
ResponseGenerator::generate_response(const RequestInfo &request_info,
                                     const bool         is_connection_close,
                                     HttpStatusCode::StatusCode status_code) {
  std::string target_path;
  if (request_info.location_ == NULL) {
    status_code = HttpStatusCode::NOT_FOUND_404;
  } else if (is_error_status_code(status_code)) {
    // LOG("status_code: " << status_code);
  } else if (request_info.location_->return_map_.size() != 0) {
    status_code = static_cast<HttpStatusCode::StatusCode>(
        request_info.location_->return_map_.begin()->first);
  } else {
    ResponseInfo result = _handle_method(request_info);
    status_code         = result.status_code_;
    target_path         = result.target_path_;
  }
<<<<<<< HEAD
  std::string body     = _create_body(request_info, status_code, target_path);
  std::string response = create_response_message(
      request_info, is_connection_close, status_code, body);
=======
  std::string response = response_message(request_info, is_connection_close,
                                          status_code, target_path);
>>>>>>> 6deebcff37bd2787277d6d3292c6961fc9815cd4
  return Response(response, is_connection_close);
}
