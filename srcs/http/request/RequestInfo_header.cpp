#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>

#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

// 呼び出し元で例外をcatchする
// リクエストヘッダのパースが終了 true。エラー→例外
void RequestInfo::parse_request_header() {
  // call each field's parser
  __parse_request_host();           // throws BadRequestException
  __parse_request_connection();     // noexcept
  __parse_request_content_length(); // noexcept
  __parse_request_transfer_encoding();
}

void RequestInfo::store_request_header_field_map(
    const std::string &header_line) {
  std::size_t pos = header_line.find(':');
  if (pos == std::string::npos) {
    throw BadRequestException();
  }
  std::string field_name = header_line.substr(0, pos);
  char        last_char  = field_name[field_name.size() - 1];
  if (last_char == ' ' || last_char == '\t') {
    throw BadRequestException();
  }
  std::string field_value =
      __trim_optional_whitespace(header_line.substr(pos + 1));
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

// TODO: Transfer-Encodingはカンマ区切りのフィールド
void RequestInfo::__parse_request_transfer_encoding() {
  if (!__field_map_.count("Transfer-Encoding")) {
    return;
  }
  std::string value = __field_map_["Transfer-Encoding"];
  if (value == "chunked") {
    is_chunked_ = true;
  }
}
