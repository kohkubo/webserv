#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>

#include "utils/utils.hpp"

#define CRLF "\r\n"

void RequestInfo::parse_request_body(std::string      &request_body,
                                     const MultiField &content_type) {
  if (content_type.first_ == "application/x-www-form-urlencoded") {
    __parse_request_values(request_body);
  } else if (content_type.first_ == "multipart/form-data") {
    __parse_request_files(request_body);
  } else if (content_type.first_ == "text/plain") {
    env_values_.push_back(request_body); // tmp
  } else {
    ERROR_LOG("unknown content-type:" + content_type.first_);
    throw RequestInfo::BadRequestException(NOT_IMPLEMENTED_501); // tmp
  }
}

// static, 引数
void RequestInfo::__parse_request_values(const std::string &request_body) {
  tokenVector tokens = tokenize(request_body, "&", "&");
  for (tokenIterator it = tokens.begin(); it != tokens.end(); ++it) {
    // TODO: 今のところ 読み取り文字数を無視して文字列を取り込みしています。
    // TODO: "hoge=huga"の形になっているかのバリデート必要かも
    env_values_.push_back(*it);
  }
}

static void mess(const std::string theme, const std::string mess) {
  std::cout << "====" << theme << "====" << std::endl
            << mess << "========"
            << "\n\n\n";
}

// static, 引数
void RequestInfo::__parse_request_files(const std::string &request_body) {
  std::map<std::string, std::string>::const_iterator it;
  it = content_type_.parameter_.find("boundary");
  if (it == content_type_.parameter_.end() || it->second == "") {
    ERROR_LOG("missing boundary");
    throw RequestInfo::BadRequestException(NOT_IMPLEMENTED_501); // tmp
  }
  std::string boundary = "--" + it->second + CRLF;
  std::string end      = "--" + it->second + "--" CRLF;
  std::string body     = request_body;
  mess("body", request_body);
  mess("boundary", boundary);
  while (true) {
    std::size_t pos = 0;
    if ((pos = body.find(boundary)) != std::string::npos) {
      __parse_multipar_body(body.substr(0, pos));
      body.erase(0, pos + boundary.size());
    } else if ((pos = body.find(end)) != std::string::npos) {
      __parse_multipar_body(body.substr(0, pos));
      break;
    } else {
      ERROR_LOG("missing end boundary");
      throw RequestInfo::BadRequestException(NOT_IMPLEMENTED_501); // tmp
    }
  }
}

// transactionの機能と共有
static bool getline(std::string &request_buffer, std::string &line) {
  std::size_t pos = request_buffer.find(CRLF);
  if (pos == std::string::npos)
    return false;
  line = request_buffer.substr(0, pos);
  request_buffer.erase(0, pos + 2);
  return true;
}

void RequestInfo::__parse_multipar_body(std::string part_body) {
  mess("origin part", part_body);
  std::map<std::string, std::string> field_map;
  RequestInfo::MultiField            desposition;
  std::string                        body;
  std::string                        line;
  while (getline(part_body, line)) {
    if (line == "") {
      break;
    }
    RequestInfo::store_request_header_field_map(line, field_map);
  }
  desposition =
      RequestInfo::__parse_multi_field(field_map["Content-Disposition"]);
  body = part_body;
  mess("part desp first", desposition.first_);
  std::map<std::string, std::string>::iterator it =
      desposition.parameter_.begin();
  for (; it != desposition.parameter_.end(); it++) {
    mess("part desp parme", it->first + ": " + it->second);
  }
  mess("part body", body);
}
