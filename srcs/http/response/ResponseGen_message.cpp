#include "http/response/ResponseGenerator.hpp"

#include <fcntl.h>

#include "http/const/const_delimiter.hpp"
#include "http/const/const_status_phrase.hpp"
#include "utils/file_io_utils.hpp"

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

std::string ResponseGenerator::_create_default_body_content(
    HttpStatusCode::StatusCode status_code) {
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

static Result<int> open_file(const std::string &target_path) {
  int fd = open(target_path.c_str(), O_RDONLY);
  if (fd < 0) {
    ERROR_LOG("open error: " << target_path);
    return Error<int>();
  }
  return Ok<int>(fd);
}

ResponseGenerator::Body ResponseGenerator::_create_status_code_body(
    const RequestInfo &request_info, HttpStatusCode::StatusCode status_code) {
  ResponseGenerator::Body body;
  body.status_code_ = status_code;
  errorPageMap::const_iterator it =
      request_info.config_.error_pages_.find(body.status_code_);
  if (it != request_info.config_.error_pages_.end()) {
    std::string file_path = request_info.location_->root_ + it->second;
    if (!is_file_exists(file_path)) {
      if (body.status_code_ == HttpStatusCode::NOT_FOUND_404) {
        return body;
      }
      return _create_status_code_body(request_info,
                                      HttpStatusCode::NOT_FOUND_404);
    }
    Result<int> result = open_file(file_path);
    if (result.is_err_) {
      if (body.status_code_ == HttpStatusCode::INTERNAL_SERVER_ERROR_500) {
        return body;
      }
      return _create_status_code_body(
          request_info, HttpStatusCode::INTERNAL_SERVER_ERROR_500);
    }
    body.fd_ = result.object_;
  }
  return body;
}

static std::string start_line(const HttpStatusCode::StatusCode status_code) {
  return "HTTP/1.1" + SP + g_response_status_phrase_map[status_code] + CRLF;
}

static std::string connection_header() { return "Connection: close" + CRLF; }

static std::string location_header(const std::string &path) {
  return "Location: " + path + CRLF;
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
    returnMap::const_iterator it =
        request_info.location_->return_map_.find(status_code);
    response += location_header(it->second);
  }
  if (HttpStatusCode::Created_201 == status_code) {
    response += location_header("target_path");
  }
  return response;
}

std::string ResponseGenerator::_create_response_message(
    const RequestInfo &request_info, const bool is_connection_close,
    HttpStatusCode::StatusCode status_code, const std::string &body) {
  std::string response =
      create_response_header(request_info, is_connection_close, status_code);
  response += entity_header_and_body(body);
  return response;
};
