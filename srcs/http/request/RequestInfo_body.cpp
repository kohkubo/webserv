#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>

#include "event/Request.hpp"
#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

static void mess(const std::string theme, const std::string mess) {
  std::cout << "====" << theme << "====" << std::endl
            << mess << "========"
            << "\n\n\n";
}

static void put_multiform(RequestInfo::MultiForm f) {
  // clang-format off
  RequestInfo::MultiForm::iterator it = f.begin();
  for (; it != f.end(); it++) {
    std::string a;
    a += "content_dispostion:\n";
    a += it->content_disposition_.type_ + "\n";
    a += it->content_disposition_.parameter_["name"] + "\n";
    a += it->content_disposition_.parameter_["filename"] + "\n";
    a += "content_type:\n";
    a += it->content_type_.type_ + "\n";
    a += "content:\n";
    a += it->content_;
    mess("part", a);
  }
  // clang-format on
}

void RequestInfo::parse_request_body(std::string       &request_body,
                                     const ContentInfo &content_type) {
  if (content_type.type_ == "application/x-www-form-urlencoded") {
    env_values_ = __parse_request_envvalues(request_body);
  } else if (content_type.type_ == "multipart/form-data") {
    multi_part_form_ = __parse_request_multiform(request_body, content_type);
  } else if (content_type.type_ == "text/plain") {
    ERROR_LOG("not support content-type:" + content_type.type_);
    throw RequestInfo::BadRequestException(NOT_IMPLEMENTED_501); // tmp
  } else {
    ERROR_LOG("unknown content-type:" + content_type.type_);
    throw RequestInfo::BadRequestException(NOT_IMPLEMENTED_501); // tmp
  }
  put_multiform(multi_part_form_);
}

// TODO: 今のところ 読み取り文字数を無視して文字列を取り込みしています。
// TODO: =があるかなどのバリデート必要か
RequestInfo::EnvValues
RequestInfo::__parse_request_envvalues(const std::string &request_body) {
  RequestInfo::EnvValues res;
  tokenVector            tokens = tokenize(request_body, "&", "&");
  for (tokenIterator it = tokens.begin(); it != tokens.end(); ++it) {
    res.push_back(*it);
  }
  return res;
}

RequestInfo::MultiForm
RequestInfo::__parse_request_multiform(std::string       &request_body,
                                       const ContentInfo &content_type) {
  std::map<std::string, std::string>::const_iterator it;
  it = content_type.parameter_.find("boundary");
  if (it == content_type.parameter_.end() || it->second == "") {
    ERROR_LOG("missing boundary");
    throw RequestInfo::BadRequestException(NOT_IMPLEMENTED_501); // tmp
  }
  std::string boundary = "--" + it->second + CRLF;
  std::string end      = "--" + it->second + "--" + CRLF;
  mess("request_body", request_body);
  mess("boundary", boundary);
  MultiForm   mpf;
  Form        f;
  std::size_t pos = 0;
  while (true) {
    if ((pos = request_body.find(boundary)) != std::string::npos) {
      if (pos != 0) {
        mpf.push_back(__parse_form(request_body.substr(0, pos)));
      }
      request_body.erase(0, pos + boundary.size());
    } else if ((pos = request_body.find(end)) != std::string::npos) {
      mpf.push_back(__parse_form(request_body.substr(0, pos)));
      break;
    } else {
      ERROR_LOG("missing end boundary");
      throw RequestInfo::BadRequestException(NOT_IMPLEMENTED_501); // tmp
    }
  }
  return mpf;
}

// TODO: RFCのmutipartをもう一度読む, エラー処理を挟む
// TODO: 各partのnameが被ったらどうするか
// TODO: contentの最後は改行が付け足されているかも
RequestInfo::Form RequestInfo::__parse_form(std::string part_body) {
  std::string                        line;
  std::map<std::string, std::string> field_map;
  while (Request::getline(part_body, line) && line != "") {
    store_request_header_field_map(line, field_map);
  }
  Form f;
  f.content_disposition_ =
      __parse_content_info(field_map["Content-Disposition"]); // 無い時
  if (f.content_disposition_.type_ != "form-data") {
    ERROR_LOG("not support except form-data");
    throw BadRequestException(NOT_IMPLEMENTED_501); // tmp
  }
  f.content_type_ = __parse_content_info(field_map["Content-Type"]); // 無い時
  f.content_      = part_body;
  return f;
}
