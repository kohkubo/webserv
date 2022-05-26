#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>

#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

bool RequestInfo::has_request_header(const std::string &request_buffer) {
  std::size_t pos = request_buffer.find(HEADER_SP);
  if (pos == std::string::npos) {
    return false;
  }
  return true;
}

std::string RequestInfo::cut_request_header(std::string &request_buffer) {
  std::size_t pos = request_buffer.find(HEADER_SP);
  return __cut_buffer(request_buffer, pos + HEADER_SP.size());
}

// 呼び出し元で例外をcatchする
// リクエストヘッダのパースに成功 true、失敗 false。エラー→例外
void RequestInfo::parse_request_header(const std::string &request_header) {
  tokenVector fields = tokenize(request_header, CRLF, CRLF);
  __create_header_map(fields.begin(), fields.end());
  // call each field's parser
  __parse_request_host();
  __parse_request_connection();
  __parse_request_content_length();
}

void RequestInfo::__create_header_map(tokenIterator it, tokenIterator end) {
  for (; it != end; it++) {
    std::string line = *it;
    std::size_t pos  = line.find(':');
    if (pos == std::string::npos) {
      throw BadRequestException();
    }
    std::string field_name = line.substr(0, pos);
    char        last_char  = field_name[field_name.size() - 1];
    if (last_char == ' ' || last_char == '\t') {
      throw BadRequestException();
    }
    std::string field_value = __trim_optional_whitespace(line.substr(pos + 1));
    if (__field_map_.count(field_name)) {
      if (__is_comma_sparated(field_name)) {
        __field_map_[field_name] += ", " + field_value;
      } else {
        throw BadRequestException();
      }
    } else {
      __field_map_[field_name] = field_value;
    }
  }
}

// TODO: hostとportで分ける必要あるか確認
void RequestInfo::__parse_request_host() {
  if (!__field_map_.count("Host")) {
    throw BadRequestException();
  }
  std::size_t pos = __field_map_["Host"].find(':');
  if (pos == std::string::npos) {
    host_ = __field_map_["Host"];
    port_ = std::string("80");
    return;
  }
  host_        = __field_map_["Host"].substr(0, pos);
  port_        = __field_map_["Host"].substr(pos + 1);
  int port_num = atoi(__field_map_["Host"].substr(pos + 1).c_str());
  if (port_num < 0 || port_num > 65535) {
    throw BadRequestException();
  }
}

void RequestInfo::__parse_request_connection() {
  if (!__field_map_.count("Connection")) {
    return;
  }
  std::string value = tolower(__field_map_["Connection"]);
  if (value == "close") {
    is_close_ = true;
  }
}

void RequestInfo::__parse_request_content_length() {
  if (!__field_map_.count("Content-Length")) {
    return;
  }
  content_length_ = atoi(__field_map_["Content-Length"].c_str());
}
