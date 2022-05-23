#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>

#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

RequestInfo::BadRequestException::BadRequestException(const std::string &msg)
    : logic_error(msg) {}

// 呼び出し元で例外をcatchする
void RequestInfo::parse_request_header(const std::string &request_string) {
  tokenVector   fields = tokenize(request_string, CRLF, CRLF);
  tokenIterator it     = fields.begin();

  __parse_request_line(*it++);
  __create_header_map(it, fields.end());

  // call each field's parser
  __parse_request_host();
  __parse_request_connection();
  __parse_request_content_length();
  __parse_request_content_type();
}

// request-line = method SP request-target SP HTTP-version (CRLF)
void RequestInfo::__parse_request_line(const std::string &request_line) {
  std::size_t first_sp = request_line.find_first_of(' ');
  std::size_t last_sp  = request_line.find_last_of(' ');
  if (first_sp == std::string::npos || first_sp == last_sp) {
    throw BadRequestException();
  }
  std::string method_str = request_line.substr(0, first_sp);
  method_                = __parse_request_method(method_str);
  uri_     = request_line.substr(first_sp + 1, last_sp - (first_sp + 1));
  version_ = request_line.substr(last_sp + 1);
}

HttpMethod RequestInfo::__parse_request_method(const std::string &method) {
  if (method == "GET") {
    return GET;
  }
  if (method == "POST") {
    return POST;
  }
  if (method == "DELETE") {
    return DELETE;
  }
  return UNKNOWN;
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

bool RequestInfo::__is_comma_sparated(std::string &field_name) {
  (void)field_name;
  // カンマ区切りが定義されたフィールドか判定する。
  // tmp
  return false;
}

std::string RequestInfo::__trim_optional_whitespace(std::string str) {
  str.erase(0, str.find_first_not_of(" \t"));
  std::size_t pos = str.find_last_not_of(" \t");
  if (pos != std::string::npos) {
    str.erase(pos + 1);
  }
  return str;
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

// TODO: content-typeはどのtypeまで把握する必要があるのか確認
static ContentType content_type(const std::string &type) {
  if (type == "application/x-www-form-urlencoded") {
    return URLENCODED;
  }
  return SOMETYPE;
}

void RequestInfo::__parse_request_content_type() {
  if (!__field_map_.count("Content-Type")) {
    return;
  }
  content_type_ = content_type(__field_map_["Content-Type"].c_str());
}
