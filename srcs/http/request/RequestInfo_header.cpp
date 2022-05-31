#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

// 呼び出し元で例外をcatchする
// リクエストヘッダのパースが終了 true。エラー→例外
void RequestInfo::parse_request_header(
    const std::map<std::string, std::string> &header_field_map) {
  // call each field's parser
  std::map<std::string, std::string>::const_iterator itr;
  itr = header_field_map.find("Host");
  if (itr == header_field_map.end()) {
    // TODO:
    // この例外処理、header_field_mapを格納し終わった後にすれば、header_field_mapをメンバ変数として持たなくて良い気がする
    throw BadRequestException("Host field is not found.");
  }
  host_ = __parse_request_host(itr->second);
  itr   = header_field_map.find("Connection");
  if (itr != header_field_map.end()) {
    is_close_ = __parse_request_connection(itr->second);
  }
  itr = header_field_map.find("Content-Length");
  if (itr != header_field_map.end()) {
    content_length_ = __parse_request_content_length(itr->second);
  }
  itr = header_field_map.find("Transfer-Encoding");
  if (itr != header_field_map.end()) {
    is_chunked_ = __parse_request_transfer_encoding(itr->second);
  }
  itr = header_field_map.find("Content-Type");
  if (itr != header_field_map.end()) {
    content_type_ = __parse_request_content_type(itr->second);
  }
}

void RequestInfo::store_request_header_field_map(
    const std::string                  &header_line,
    std::map<std::string, std::string> &header_field_map) {
  std::size_t pos = header_line.find(':');
  if (pos == std::string::npos) {
    throw BadRequestException();
  }
  std::string field_name = header_line.substr(0, pos);
  char        last_char  = field_name[field_name.size() - 1];
  if (last_char == ' ' || last_char == '\t') {
    throw BadRequestException();
  }
  const std::string field_value =
      trim_optional_whitespace(header_line.substr(pos + 1), " \t");
  if (header_field_map.count(field_name) != 0u) {
    if (__is_comma_sparated(field_name)) {
      header_field_map[field_name] += ", " + field_value;
    } else {
      throw BadRequestException();
    }
  } else {
    header_field_map[field_name] = field_value;
  }
}

// TODO: hostとportで分ける必要あるか確認
std::string RequestInfo::__parse_request_host(const std::string &host_lien) {
  std::size_t pos = host_lien.find(':');
  if (pos == std::string::npos) {
    return host_lien;
  }
  return host_lien.substr(0, pos);
}

bool RequestInfo::__parse_request_connection(const std::string &connection) {
  // TODO: tolower ってことは cLoseとかもあり? kohkubo
  return tolower(connection) == "close";
}

size_t
RequestInfo::__parse_request_content_length(const std::string &content_length) {
  // TODO: atoiのエラー処理
  return atoi(content_length.c_str());
}

// TODO: Transfer-Encodingはカンマ区切りのフィールド
bool RequestInfo::__parse_request_transfer_encoding(
    const std::string &transfer_encoding) {
  return transfer_encoding == "chunked";
}

ContentType
RequestInfo::__parse_request_content_type(const std::string &content_type) {
  std::string content_type_lower = tolower(content_type);
  if (content_type_lower == "application/x-www-form-urlencoded") {
    return CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED;
  } else if (content_type_lower == "multipart/form-data") {
    return CONTENT_TYPE_MULTIPART_FORM_DATA;
  } else if (content_type_lower == "text/plain") {
    return CONTENT_TYPE_TEXT_PLAIN;
  }
  return CONTENT_TYPE_UNKNOWN;
}