#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>

#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

static void mess(const std::string theme, const std::string mess) {
  std::cout << "====" << theme << "====" << std::endl
            << mess << "========"
            << "\n\n\n";
}

void RequestInfo::parse_request_body(std::string       &request_body,
                                     const ContentInfo &content_type) {
  if (content_type.type_ == "application/x-www-form-urlencoded") {
    __parse_request_envvalues(request_body);
  } else if (content_type.type_ == "multipart/form-data") {
    __parse_request_files(request_body);
  } else if (content_type.type_ == "text/plain") {
    env_values_.push_back(request_body); // tmp
  } else {
    ERROR_LOG("unknown content-type:" + content_type.type_);
    throw RequestInfo::BadRequestException(NOT_IMPLEMENTED_501); // tmp
  }
  MultiPart::iterator it = multi_part_.begin();
  for (; it != multi_part_.end(); it++) {
    std::string a;
    a += "name: " + it->first + "\n";
    a += "filename: " + it->second.filename_ + "\n";
    a += "contenttype: " + it->second.content_type_ + "\n";
    a += "filecontent :" + it->second.content_ + "\n";
    mess("part", a);
  }
}

// static, 引数
void RequestInfo::__parse_request_envvalues(const std::string &request_body) {
  tokenVector tokens = tokenize(request_body, "&", "&");
  for (tokenIterator it = tokens.begin(); it != tokens.end(); ++it) {
    // TODO: 今のところ 読み取り文字数を無視して文字列を取り込みしています。
    // TODO: "hoge=huga"の形になっているかのバリデート必要かも
    env_values_.push_back(*it);
  }
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
  std::string end      = "--" + it->second + "--" + CRLF;
  std::string body     = request_body;
  mess("body", request_body);
  mess("boundary", boundary);
  std::size_t pos = 0;
  while (true) {
    if ((pos = body.find(boundary)) != std::string::npos) {
      __parse_formdata(body.substr(0, pos));
      body.erase(0, pos + boundary.size());
    } else if ((pos = body.find(end)) != std::string::npos) {
      __parse_formdata(body.substr(0, pos));
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

// TODO: keyが被ったらどうするか
void RequestInfo::__parse_formdata(std::string part_body) {
  std::map<std::string, std::string> field_map;
  RequestInfo::ContentInfo           content_disposition;
  RequestInfo::ContentInfo           content_type;
  std::string                        line;
  if (part_body == "") {
    return;
  }
  while (getline(part_body, line)) {
    if (line == "") {
      break;
    }
    RequestInfo::store_request_header_field_map(line, field_map);
  }
  content_disposition = RequestInfo::__parse_content_info(
      field_map["Content-Disposition"]); // 無い時
  if (content_disposition.type_ != "form-data") {
    ERROR_LOG("not support except form-data");
    throw RequestInfo::BadRequestException(NOT_IMPLEMENTED_501); // tmp
  }
  content_type =
      RequestInfo::__parse_content_info(field_map["Content-Type"]); // 無い時
  FormData    f;
  std::string formkey = content_disposition.parameter_["name"]; // TODO: 無い時
  f.filename_         = content_disposition.parameter_["filename"];
  f.content_type_     = content_type.type_;
  f.content_          = part_body;
  multi_part_[formkey] = f;
}
