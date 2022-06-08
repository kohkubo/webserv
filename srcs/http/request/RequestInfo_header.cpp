#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

#include "http/const/const_delimiter.hpp"
#include "utils/tokenize.hpp"
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
    throw BadRequestException(BAD_REQUEST_400, "Host field is not found.");
  }
  host_ = __parse_request_host(itr->second);
  itr   = header_field_map.find("Connection");
  if (itr != header_field_map.end()) {
    connection_close_ = __parse_request_connection(itr->second);
  }
  itr = header_field_map.find("Content-Length");
  if (itr != header_field_map.end()) {
    content_length_ = __parse_request_content_length(itr->second);
  }
  itr = header_field_map.find("Transfer-Encoding");
  if (itr != header_field_map.end()) {
    is_chunked_ = __parse_request_transfer_encoding(itr->second);
  }
  // Content-Type: multipart/form-data;boundary="boundary"
  // media-type = type "/" subtype *( OWS ";" OWS parameter )
  itr = header_field_map.find("Content-Type");
  if (itr != header_field_map.end()) {
    tokenVector   token_vector = tokenize(itr->second, ";", ";");
    tokenIterator it           = token_vector.begin();
    for (; it != token_vector.end(); it++) {
      std::string str = __trim_optional_whitespace(*it);
      if (content_type_ == "") {
        content_type_ = tolower(str);
        // typeの先頭のOWSは許容してしまう
        // 英字以外も小文字になってしまう？
      } else { // parameter
        std::size_t pos = str.find('=');
        if (pos == std::string::npos) {
          throw BadRequestException();
        }
        std::string key   = tolower(str.substr(0, pos));
        std::string value = str.substr(pos + 1);
        if (value[0] == '\"') {
          std::size_t pos = value.find_last_of("\"");
          if (pos == 0 || !(value.size() - 1 == pos)) {
            throw BadRequestException();
          }
          value = value.substr(1, pos - 1); // ""に囲まれたものはlowerにしない
        } else {
          value = tolower(value);
        }
        parameter_[key] = value;
      }
    }
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
      __trim_optional_whitespace(header_line.substr(pos + 1));
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
std::string RequestInfo::__parse_request_host(const std::string &host_line) {
  std::size_t pos = host_line.find(':');
  if (pos == std::string::npos) {
    return host_line;
  }
  return host_line.substr(0, pos);
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

std::string RequestInfo::__trim_optional_whitespace(std::string str) {
  str.erase(0, str.find_first_not_of(" \t"));
  std::size_t pos = str.find_last_not_of(" \t");
  if (pos != std::string::npos) {
    str.erase(pos + 1);
  }
  return str;
}
