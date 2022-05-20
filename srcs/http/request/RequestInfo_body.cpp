#include "http/request/RequestInfo.hpp"

static std::pair<std::string, std::string>
parse_request_value(std::string value) {
  std::string::size_type pos = value.find('=');
  if (pos == std::string::npos) {
    return std::make_pair("", "");
  }
  return std::make_pair(value.substr(0, pos), value.substr(pos + 1));
}

// TODO: 今のところ Content-Type は無視している
// TODO: 今のところ 一列でくるものと仮定
void RequestInfo::parse_request_body(const std::string &request_body) {
  tokenVector tokens = tokenize(request_body, "&", "&");
  for (tokenIterator it = tokens.begin(); it != tokens.end(); ++it) {
    // TODO: 今のところ 読み取り文字数を無視して文字列を取り込みしています。
    std::pair<std::string, std::string> value = parse_request_value(*it);
    values_[value.first]                      = value.second;
  }
}
