#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

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
    env_values_.push_back(request_body); // tmp, gtestに関わるので変更後テスト
  } else {
    ERROR_LOG("unknown content-type:" + content_type.type_);
    throw RequestInfo::BadRequestException(NOT_IMPLEMENTED_501); // tmp
  }
  put_multiform(multi_part_form_);
}

// TODO: 今のところ 読み取り文字数を無視して文字列を取り込みしています。
// TODO: =があるかなどのバリデート必要か
// TODO: 英数字以外は%エンコーディングされている(mdn POST), 考慮してない
RequestInfo::EnvValues
RequestInfo::__parse_request_envvalues(const std::string &request_body) {
  RequestInfo::EnvValues res;
  tokenVector            tokens = tokenize(request_body, "&", "&");
  for (tokenIterator it = tokens.begin(); it != tokens.end(); ++it) {
    res.push_back(*it);
  }
  return res;
}

// RFC7578にはboundary・CRLF・"--"で区切られる程度しか書いてないので
// 実際にpostしてみた時の受信ボディに合わせてパースした
static std::vector<std::string>
tokenize_multiform(std::string request_body, const std::string &boundary) {
  std::string first_boundary  = "--" + boundary + CRLF;
  std::string middle_boundary = CRLF + "--" + boundary + CRLF;
  std::string end_boundary    = CRLF + "--" + boundary + "--" + CRLF;
  std::size_t pos             = request_body.find(first_boundary);
  if (pos != 0) {
    ERROR_LOG("part body: didn't start at the boundary");
    throw RequestInfo::BadRequestException();
  }
  request_body.erase(0, pos + first_boundary.size());
  std::vector<std::string> res;
  while (true) {
    if ((pos = request_body.find(middle_boundary)) != std::string::npos) {
      res.push_back(request_body.substr(0, pos));
      request_body.erase(0, pos + middle_boundary.size());
    } else if ((pos = request_body.find(end_boundary)) != std::string::npos) {
      res.push_back(request_body.substr(0, pos));
      break;
    } else {
      ERROR_LOG("part body: missing end boundary");
      throw RequestInfo::BadRequestException();
    }
  }
  return res;
}

// 現状, MultiFormは簡素化のためFormのvector
// 本来は各パートのフィールド名(nameで指定される)をkey
// 各パートのForm情報をvalueとするmapが良いかも
// keyは被ったら無視(RFC7578-3)
RequestInfo::MultiForm
RequestInfo::__parse_request_multiform(const std::string &request_body,
                                       const ContentInfo &content_type) {
  std::map<std::string, std::string>::const_iterator it;
  it = content_type.parameter_.find("boundary");
  if (it == content_type.parameter_.end() || it->second == "") {
    ERROR_LOG("content-type: missing boundary");
    throw RequestInfo::BadRequestException();
  }
  mess("request_body", request_body);
  mess("boundary", it->second);
  std::vector<std::string> tokens =
      tokenize_multiform(request_body, it->second);
  MultiForm                          multi_form;
  std::vector<std::string>::iterator itm = tokens.begin();
  for (; itm != tokens.end(); itm++) {
    multi_form.push_back(__parse_request_form(*itm));
  }
  return multi_form;
}

// ファイル名の%エンコード(RFC7578-2)は考慮してない
// フィールド名のascii制限(RFC7578-5)もとりあえず無視
// multipartで送られるファイルが全てstringに収まること前提で良いのか
// 現状, content-typeはデフォルトのtext/plain; charset=US-ASCIIであること前提
// コンテンツがfileの場合でも, filenameが指定されていない場合がある(RFC7578-4.2)
// filenameはそのまま使わずに, 場合(破壊的なパスなど)によっては変更する
RequestInfo::Form RequestInfo::__parse_request_form(std::string part_body) {
  std::string                        line;
  std::map<std::string, std::string> field_map;
  while (Request::getline(part_body, line) && line != "") {
    store_request_header_field_map(line, field_map);
  }
  std::map<std::string, std::string>::iterator it;
  it = field_map.find("Content-Disposition");
  if (it == field_map.end()) {
    ERROR_LOG("part header: missing Content-Disposition");
    throw RequestInfo::BadRequestException();
  }
  Form form;
  form.content_disposition_ = __parse_content_info(it->second);
  if (form.content_disposition_.type_ != "form-data") {
    ERROR_LOG("part header: type must be form-data");
    throw BadRequestException();
  }
  std::map<std::string, std::string>::iterator itn;
  itn = form.content_disposition_.parameter_.find("name");
  if (itn == field_map.end()) {
    ERROR_LOG("part header: missing name");
    throw RequestInfo::BadRequestException();
  }
  form.content_type_ = __parse_content_info(field_map["Content-Type"]);
  form.content_      = part_body;
  return form;
}
