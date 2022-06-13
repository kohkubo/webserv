#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

#include "event/Request.hpp"
#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

void RequestInfo::parse_request_body(std::string       &request_body,
                                     const ContentInfo &content_type) {
  if (content_type.type_ == "application/x-www-form-urlencoded") {
    env_values_ = __parse_request_env_values(request_body);
  } else if (content_type.type_ == "multipart/form-data") {
    multi_form_ = __parse_request_multi_form(request_body, content_type);
  } else if (content_type.type_ == "text/plain") {
    env_values_.push_back(request_body); // tmp, gtestに関わるので変更後テスト
  } else {
    ERROR_LOG("unknown content-type:" + content_type.type_);
    throw RequestInfo::BadRequestException(NOT_IMPLEMENTED_501); // tmp
  }
}

// TODO: 今のところ 読み取り文字数を無視して文字列を取り込みしています。
// TODO: =があるかなどのバリデート必要か
// TODO: 英数字以外は%エンコーディングされている(mdn POST), 考慮してない
RequestInfo::EnvValues
RequestInfo::__parse_request_env_values(const std::string &request_body) {
  RequestInfo::EnvValues res;
  tokenVector            tokens = tokenize(request_body, "&", "&");
  for (tokenIterator it = tokens.begin(); it != tokens.end(); ++it) {
    res.push_back(*it);
  }
  return res;
}

// TODO:
//  現状, MultiFormは簡素化のためFormのvector.
//  本来は各パートのフィールド名(nameで指定される)をkey
//  各パートのForm情報をvalueとするmapが良いかも.
//  keyは被ったら無視(RFC7578-3).
RequestInfo::MultiForm
RequestInfo::__parse_request_multi_form(std::string        request_body,
                                        const ContentInfo &content_type) {
  std::map<std::string, std::string>::const_iterator it;
  it = content_type.parameter_.find("boundary");
  if (it == content_type.parameter_.end() || it->second == "") {
    ERROR_LOG("content-type: missing boundary");
    throw RequestInfo::BadRequestException();
  }
  enum MultiFormState {
    BEGINNING,
    RECEIVING_HEADER,
    RECEIVING_BODY,
  };
  std::string    boundary = "--" + it->second;
  std::string    end      = "--" + it->second + "--";
  std::string    line;
  MultiFormState s = BEGINNING;
  MultiForm      multi_form;
  Form           form;
  while (must_get_line(request_body, line)) {
    if (line == boundary) {
      if (s != BEGINNING) {
        __add_form_to_multi_form(multi_form, form);
      }
      form = Form();
      s    = RECEIVING_HEADER;
      continue;
    }
    if (line == end) {
      __add_form_to_multi_form(multi_form, form);
      break;
    }
    if (line == "") {
      s = RECEIVING_BODY;
      continue;
    }
    if (s == RECEIVING_HEADER) {
      __parse_form_header(line, form);
    } else if (s == RECEIVING_BODY) {
      if (form.content_ != "") {
        // これ以前のcontentが空でないなら,
        // bodyの途中で改行がある=CRLF付け足す
        form.content_ += CRLF;
      }
      form.content_ += line;
    }
  }
  return multi_form;
}

// TODO: ファイル名の%エンコード(RFC7578-2)は考慮してない
// TODO: フィールド名のascii制限(RFC7578-5)もとりあえず無視
// TODO: ボディは一つ以上ないといけない
// TODO: 現状, content-typeはデフォルトのtext/plain;
// charset=US-ASCIIであること前提
// TODO: コンテンツがfileの場合でも,
// filenameが指定されていない場合がある(RFC7578-4.2)
// TODO: filenameはそのまま使わずに, 場合(破壊的なパスなど)によっては変更する
void RequestInfo::__parse_form_header(const std::string  line,
                                      RequestInfo::Form &form) {
  std::size_t pos = line.find(':');
  if (pos == std::string::npos) {
    throw BadRequestException();
  }
  const std::string field_name = line.substr(0, pos);
  char              last_char  = field_name[field_name.size() - 1];
  if (last_char == ' ' || last_char == '\t') {
    throw BadRequestException();
  }
  const std::string field_value = trim(line.substr(pos + 1), ows_);
  if (field_name == "Content-Disposition" &&
      form.content_disposition_.type_ == "") {
    form.content_disposition_ = __parse_content_info(field_value);
  }
  if (field_name == "Content-Type" && form.content_type_.type_ == "") {
    form.content_type_ = __parse_content_info(field_value);
  }
}

void RequestInfo::__add_form_to_multi_form(MultiForm  &multi_form,
                                           const Form &form) {
  if (form.content_disposition_.type_ != "form-data") {
    ERROR_LOG("part header: type must be form-data");
    throw BadRequestException();
  }
  std::map<std::string, std::string>::const_iterator it =
      form.content_disposition_.parameter_.find("name");
  if (it == form.content_disposition_.parameter_.end()) {
    ERROR_LOG("part header: missing name");
    throw RequestInfo::BadRequestException();
  }
  multi_form.push_back(form);
}
