#include "http/request/RequestInfo.hpp"

static ContentType parse_content_type(const std::string &type) {
  if (type == "application/x-www-form-urlencoded") {
    return URLENCODED;
  }
  return NOTSUPPORTED;
}

static std::pair<std::string, std::string>
parse_request_value(std::string value) {
  std::string::size_type pos = value.find('=');
  if (pos == std::string::npos) {
    return std::make_pair("", "");
  }
  return std::make_pair(value.substr(0, pos), value.substr(pos + 1));
}

// TODO: Content-Typeは何を対応するのか, そもそもパースはするのかわからない
// TODO: chunkedについても考える
void RequestInfo::parse_request_body(const std::string &request_body) {
  tokenVector tokens = tokenize(request_body, "&", "&");
  for (tokenIterator it = tokens.begin(); it != tokens.end(); ++it) {
    // TODO: 今のところ 読み取り文字数を無視して文字列を取り込みしています。
    std::pair<std::string, std::string> value = parse_request_value(*it);
    values_[value.first]                      = value.second;
  }
}
