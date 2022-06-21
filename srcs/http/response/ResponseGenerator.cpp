#include "http/response/ResponseGenerator.hpp"

#include <cstdlib>
#include <unistd.h>

#include <iostream>
#include <map>
#include <stdexcept>

#include "config/Config.hpp"
#include "config/Location.hpp"
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
  return status_code > 299 && status_code < 600;
}

// TODO: config.error_page validate
static std::string error_page_body(const RequestInfo         &request_info,
                                   HttpStatusCode::StatusCode status_code) {
  errorPageMap::const_iterator it =
      request_info.config_.error_pages_.find(status_code);
  if (it != request_info.config_.error_pages_.end()) {
    std::string file_path = request_info.location_.root_ + it->second;
    // TODO:
    // 無いエラーページを指定した場合、nginxではどのような挙動になるのかチェックが必要
    Result result         = read_file_to_str(file_path);
    if (!result.is_err_) {
      return result.str_;
    }
    LOG("error_page_body: read_file_to_str error");
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
         "default error page\n"
         "    </body>\n"
         "</html>";
}

std::string ResponseGenerator::_body(const RequestInfo &request_info) {
  if (has_suffix(request_info.target_path_, ".py")) {
    Result result = _read_file_to_str_cgi(request_info);
    if (result.is_err_) {
      return error_page_body(request_info,
                             HttpStatusCode::INTERNAL_SERVER_ERROR_500);
    }
    return result.str_;
  }
  if (has_suffix(request_info.target_path_, "/")) {
    return _create_autoindex_body(request_info);
  }
  Result result = read_file_to_str(request_info.target_path_);
  if (result.is_err_) {
    return error_page_body(request_info,
                           HttpStatusCode::INTERNAL_SERVER_ERROR_500);
  }
  return result.str_;
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

std::string ResponseGenerator::generate_error_response(
    const RequestInfo &request_info, HttpStatusCode::StatusCode status_code) {
  LOG("generate_error_response()");
  LOG("status_code: " << status_code);
  std::string response = start_line(status_code);
  response += connection_header();
  std::string body = error_page_body(request_info, status_code);
  response += entity_header_and_body(body);
  LOG(response);
  return response;
}

static std::string response_message(HttpStatusCode::StatusCode status_code,
                                    const std::string         &body,
                                    const RequestInfo         &request_info) {
  std::string response = start_line(status_code);
  if (request_info.connection_close_) {
    response += connection_header();
  }
  if (status_code == HttpStatusCode::NO_CONTENT_204) {
    return response + CRLF;
  }
  if (HttpStatusCode::MOVED_PERMANENTLY_301 == status_code) {
    response +=
        location_header(request_info.location_.return_map_, status_code);
  }
  response += entity_header_and_body(body);
  return response;
};

std::string
ResponseGenerator::generate_response(const RequestInfo &request_info) {
  HttpStatusCode::StatusCode status_code = HttpStatusCode::NONE;
  // TODO: 例外処理をここに挟むかも 2022/05/22 16:21 kohkubo nakamoto 話し合い
  // エラーがあった場合、それ以降の処理が不要なので、例外処理でその都度投げる??
  // TODO:locationを決定する処理をResponseの前に挟むと、
  // Responseクラスがconst参照としてLocationを持つことができるがどうだろう。kohkubo
  // return がセットされていたら
  if (request_info.location_.return_map_.size() != 0) {
    // intをHttpStatusCodeに変換する
    status_code = static_cast<HttpStatusCode::StatusCode>(
        request_info.location_.return_map_.begin()->first);
    return response_message(status_code, "", request_info);
  }
  status_code = _handle_method(request_info);
  if (is_error_status_code(status_code)) {
    LOG("generate_response: error status code");
    // TODO: locationの渡し方は全体の処理の流れが決まるまで保留 kohkubo
    return generate_error_response(request_info, status_code);
  }
  if (status_code == HttpStatusCode::NO_CONTENT_204) {
    // TODO:
    // 本来はここに分岐がない方がよいですが、現状のロジックだと必要なのでとりあえずの実装です
    // kohkubo
    return response_message(status_code, "", request_info);
  }
  return response_message(status_code, _body(request_info), request_info);
}
