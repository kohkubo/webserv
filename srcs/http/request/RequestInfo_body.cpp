#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

#include "event/Request.hpp"
#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

void RequestInfo::parse_request_body(std::string       &request_body,
                                     const ContentInfo &content_type) {
  if (content_type.type_ == "application/x-www-form-urlencoded" ||
      content_type.type_ == "text/plain") {
    body_ = request_body;
    return;
  }
  if (content_type.type_ == "multipart/form-data") {
    form_map_ = _parse_request_multi_part(request_body, content_type);
  } else {
    ERROR_LOG("unknown content-type:" + content_type.type_);
    throw RequestInfo::BadRequestException(NOT_IMPLEMENTED_501); // tmp
  }
}

// rakiyama
//  TODO: ファイル名の%エンコード(RFC7578-2)は考慮してない
//  TODO: フィールド名のascii制限(RFC7578-5)もとりあえず無視
//  TODO: 現状, content-typeはデフォルトのtext/plain;
//        charset=US-ASCIIであること前提
//  TODO: コンテンツがfileの場合でも,
//  filenameが指定されていない場合がある(RFC7578-4.2)
//  TODO: filenameはそのまま使わずに, 場合(破壊的なパスなど)によっては変更する
RequestInfo::formMap
RequestInfo::_parse_request_multi_part(const std::string &request_body,
                                       const ContentInfo &content_type) {
  std::map<std::string, std::string>::const_iterator it;
  it = content_type.parameter_.find("boundary");
  if (it == content_type.parameter_.end() || it->second == "") {
    ERROR_LOG("content-type: missing boundary");
    throw RequestInfo::BadRequestException();
  }
  formMap form_map = _parse_multi_part_loop(request_body, it->second);
  if (form_map.size() == 0) {
    ERROR_LOG("multipart/form-data missing body");
    throw RequestInfo::BadRequestException();
  }
  return form_map;
}

static void add_form_to_form_map(RequestInfo::formMap    &form_map,
                                 const RequestInfo::Form &form) {
  if (form.content_disposition_.type_ != "form-data") {
    ERROR_LOG("multi part header: type must be form-data");
    throw RequestInfo::BadRequestException();
  }
  std::map<std::string, std::string>::const_iterator it =
      form.content_disposition_.parameter_.find("name");
  if (it == form.content_disposition_.parameter_.end()) {
    ERROR_LOG("multi part header: missing name");
    throw RequestInfo::BadRequestException();
  }
  form_map.insert(std::make_pair(it->second, form));
}

RequestInfo::formMap
RequestInfo::_parse_multi_part_loop(std::string        body,
                                    const std::string &boudary_specified) {
  enum MultiPartState {
    BEGINNING,
    HEADER,
    BODY,
  };
  MultiPartState state    = BEGINNING;
  std::string    boundary = "--" + boudary_specified;
  std::string    end      = "--" + boudary_specified + "--";
  formMap        form_map;
  Form           form;
  std::string    line;
  while (getline(body, line)) {
    if (line == boundary) {
      if (state != BEGINNING) {
        add_form_to_form_map(form_map, form);
      }
      form  = Form();
      state = HEADER;
      continue;
    }
    if (line == end) {
      add_form_to_form_map(form_map, form);
      break;
    }
    if (line == "") {
      state = BODY;
      continue;
    }
    if (state == HEADER) {
      _parse_form_header(line, form);
    }
    if (state == BODY) {
      if (form.content_ != "") {
        form.content_ += CRLF;
      }
      form.content_ += line;
    }
  }
  return form_map;
}

void RequestInfo::_parse_form_header(const std::string  line,
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
  const std::string field_value = trim(line.substr(pos + 1), OWS_);
  // TODO: 既にformが指定されていると上書きされる rakiyama
  if (field_name == "Content-Disposition") {
    form.content_disposition_ = _parse_content_info(field_value);
  } else if (field_name == "Content-Type") {
    form.content_type_ = _parse_content_info(field_value);
  }
}
