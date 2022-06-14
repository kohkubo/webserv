#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

#include "http/const/const_delimiter.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

const std::string RequestInfo::OWS_ = " \t";

// TODO: Transfer-Encodingはカンマ区切りのフィールド
static bool
parse_request_transfer_encoding(const std::string &transfer_encoding) {
  return transfer_encoding == "chunked";
}

// TODO: hostとportで分ける必要あるか確認
static std::string parse_request_host(const std::string &host_line) {
  std::size_t pos = host_line.find(':');
  if (pos == std::string::npos) {
    return host_line;
  }
  return host_line.substr(0, pos);
}

static bool parse_request_connection(const std::string &connection) {
  // TODO: tolower ってことは cLoseとかもあり? kohkubo
  return tolower(connection) == "close";
}

static size_t parse_request_content_length(const std::string &content_length) {
  // TODO: atoiのエラー処理
  return atoi(content_length.c_str());
}

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
  host_ = parse_request_host(itr->second);
  itr   = header_field_map.find("Connection");
  if (itr != header_field_map.end()) {
    connection_close_ = parse_request_connection(itr->second);
  }
  itr = header_field_map.find("Content-Length");
  if (itr != header_field_map.end()) {
    content_length_ = parse_request_content_length(itr->second);
  }
  itr = header_field_map.find("Transfer-Encoding");
  if (itr != header_field_map.end()) {
    is_chunked_ = parse_request_transfer_encoding(itr->second);
  }
  itr = header_field_map.find("Content-Type");
  if (itr != header_field_map.end()) {
    content_type_ = __parse_content_info(itr->second);
  }
}

// TODO: フィールド追加
static bool is_comma_separated(std::string &field_name) {
  (void)field_name;
  // カンマ区切りが定義されたフィールドか判定する。
  // tmp
  return false;
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
  const std::string field_value = trim(header_line.substr(pos + 1), OWS_);
  if (header_field_map.count(field_name) != 0u) {
    if (is_comma_separated(field_name)) {
      header_field_map[field_name] += ", " + field_value;
    } else {
      throw BadRequestException();
    }
  } else {
    header_field_map[field_name] = field_value;
  }
}

// content-typeのparameter(key=value)のvalueについて切り出す関数
// ""で囲まれていれば, それらを削除
static std::string cutout_parameter_value(std::string str) {
  if (str.size() == 0) {
    return str;
  }
  std::size_t last = str.size() - 1;
  if (str[0] == '\"' || str[last] == '\"') {
    bool is_lacking_quote = (0 == last || str[0] != str[last]);
    if (is_lacking_quote) {
      throw RequestInfo::BadRequestException();
    }
    str = str.substr(1, last - 1);
  }
  return str;
}

// 例 Content-Type: multipart/form-data;boundary="boundary"
// Content-Type = media-type = type "/" subtype *( OWS ";" OWS parameter )
// parameter    = token "=" ( token / quoted-string )
// TODO: token以外の文字があったときのバリデーとはするか
// TODO: keyの被り考慮するか
// parameter(key=value)の大文字小文字を区別するかについて:
// RFC7231: valueは大文字と小文字を区別する場合としない場合があります。
// 現状はmultipart/form-dataに合わせて大小文字は区別する(引用符があれば削除するだけ)
// もし大小文字について考慮するなら,
// 各valueの中身を参照する時にそれぞれの処理でやるべきかも
RequestInfo::ContentInfo
RequestInfo::__parse_content_info(const std::string &content) {
  ContentInfo   res;
  tokenVector   tokens = tokenize(content, ";", ";");
  tokenIterator it     = tokens.begin();
  for (; it != tokens.end(); it++) {
    std::string str = trim(*it, OWS_);
    if (res.type_ == "") {
      res.type_ = tolower(str);
    } else {
      std::size_t equal_pos = str.find('=');
      if (equal_pos == std::string::npos) {
        throw BadRequestException();
      }
      std::string key     = tolower(str.substr(0, equal_pos));
      std::string value   = cutout_parameter_value(str.substr(equal_pos + 1));
      res.parameter_[key] = value;
    }
  }
  return res;
}
