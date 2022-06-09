#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>

#include "utils/utils.hpp"

#define CRLF "\r\n"

void RequestInfo::parse_request_body(std::string       &request_body,
                                     const std::string &content_type) {
  if (content_type == "application/x-www-form-urlencoded") {
    __parse_request_values(request_body);
  } else if (content_type == "multipart/form-data") {
    __parse_request_files(request_body);
  } else if (content_type == "text/plain") {
    env_values_.push_back(request_body); // tmp
  } else {
    ERROR_LOG("unknown content-type:" + content_type);
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
            << mess << "========" << std::endl;
}

// static, 引数
void RequestInfo::__parse_request_files(const std::string &request_body) {
  std::map<std::string, std::string>::const_iterator it;
  it = ctype_parameter_.find("boundary");
  if (it == ctype_parameter_.end() || it->second == "") {
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
      mess("midl", body.substr(0, pos));
      body.erase(0, pos + boundary.size());
    } else if ((pos = body.find(end)) != std::string::npos) {
      mess("last", body.substr(0, pos));
      break;
    } else {
      ERROR_LOG("missing end boundary");
      throw RequestInfo::BadRequestException(NOT_IMPLEMENTED_501); // tmp
    }
  }
}
