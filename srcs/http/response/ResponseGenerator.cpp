#include "http/response/ResponseGenerator.hpp"

#include <cstdlib>
#include <unistd.h>

#include <iostream>
#include <map>
#include <stdexcept>

#include "config/Config.hpp"
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

static bool is_error_status_code(HttpStatusCode status_code) {
  // TODO: エラーのステータスコードの扱いを決まったら再実装
  return status_code > 299 && status_code < 600;
}

static std::string create_file_path(const std::string &request_uri,
                                    const Location    &location) {
  std::string file_path;
  file_path = location.root_ + request_uri;
  if (has_suffix(file_path, "/") &&
      is_file_exists(file_path + location.index_)) {
    file_path += location.index_;
  }
  return file_path;
}

std::string ResponseGenerator::__body(const std::string &file_path,
                                      const RequestInfo &request_info) {
  if (has_suffix(file_path, ".sh")) {
    return __read_file_tostring_cgi(file_path, request_info.env_values_);
  }
  if (has_suffix(file_path, "/")) {
    return __create_autoindex_body(file_path, request_info);
  }
  return read_file_tostring(file_path);
}

// 最長マッチ
static const Location *
select_proper_location(const std::string           &request_uri,
                       const std::vector<Location> &locations) {
  // clang-format off
  std::string     path;
  const Location *ret_location = NULL;
  // clang-format on
  std::vector<Location>::const_iterator it = locations.begin();
  for (; it != locations.end(); ++it) {
    if (request_uri.find(it->location_path_) == 0) {
      if (path.size() < it->location_path_.size()) {
        path         = it->location_path_;
        ret_location = &(*it);
      }
    }
  }
  return ret_location;
}

// TODO: config.error_page validate
static std::string error_page_body(const Location      &location,
                                   const Config        &config,
                                   const HttpStatusCode status_code) {
  std::map<int, std::string>::const_iterator it =
      config.error_pages_.find(status_code);
  if (it != config.error_pages_.end()) {
    std::string file_path = location.root_ + it->second;
    return read_file_tostring(file_path);
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

static std::string start_line(const HttpStatusCode status_code) {
  return "HTTP/1.1" + SP + g_response_status_phrase_map[status_code] + CRLF;
}

static std::string connection_header(const RequestInfo &request_info) {
  if (request_info.connection_close_) {
    return "Connection: close" + CRLF;
  }
  return "Connection: keep-alive" + CRLF;
}
static std::string location_header(const Location &location,
                                   HttpStatusCode  status_code) {
  std::map<int, std::string>::const_iterator it =
      location.return_.find(status_code);
  return "Location: " + it->second + CRLF;
}

static std::string entity_header_and_body(const std::string &body) {
  return "Content-Length: " + to_string(body.size()) + CRLF +
         "Content-Type: text/html" + CRLF + CRLF + body;
}

std::string
ResponseGenerator::generate_error_response(const Location &location,
                                           const Config   &config,
                                           HttpStatusCode  status_code) {
  LOG("status_code: " << status_code);
  std::string response = start_line(status_code);
  response += "Connection: close" + CRLF;
  std::string body = error_page_body(location, config, status_code);
  response += entity_header_and_body(body);
  LOG(response);
  return response;
}

static std::string response_message(HttpStatusCode     status_code,
                                    const std::string &body,
                                    const Location    &location,
                                    const RequestInfo &request_info) {
  std::string response = start_line(status_code);
  response += connection_header(request_info);
  if (status_code == NO_CONTENT_204) {
    return response + CRLF;
  }
  if (MOVED_PERMANENTLY_301 == status_code) {
    response += location_header(location, status_code);
  }
  response += entity_header_and_body(body);
  return response;
};

std::string
ResponseGenerator::generate_response(const Config      &config,
                                     const RequestInfo &request_info) {
  HttpStatusCode  status_code = NONE;
  // TODO: 例外処理をここに挟むかも 2022/05/22 16:21 kohkubo nakamoto 話し合い
  // エラーがあった場合、それ以降の処理が不要なので、例外処理でその都度投げる??
  // TODO:locationを決定する処理をResponseの前に挟むと、
  // Responseクラスがconst参照としてLocationを持つことができるがどうだろう。kohkubo
  const Location *location =
      select_proper_location(request_info.uri_, config.locations_);
  if (location == NULL) {
    // TODO: ここ処理どうするかまとまってないのでとりあえずの処理
    return generate_error_response(Location(), config, NOT_FOUND_404);
  }
  // return がセットされていたら
  if (location->return_.size() != 0) {
    // intをHttpStatusCodeに変換する
    status_code = static_cast<HttpStatusCode>(location->return_.begin()->first);
    return response_message(status_code, "", *location, request_info);
  }
  if (is_minus_depth(request_info.uri_)) {
    return generate_error_response(*location, config, FORBIDDEN_403);
  }
  std::string file_path = create_file_path(request_info.uri_, *location);
  status_code           = __handle_method(*location, request_info, file_path);
  if (is_error_status_code(status_code)) {
    // TODO: locationの渡し方は全体の処理の流れが決まるまで保留 kohkubo
    return generate_error_response(*location, config, status_code);
  }
  return response_message(status_code, __body(file_path, request_info),
                          *location, request_info);
}
