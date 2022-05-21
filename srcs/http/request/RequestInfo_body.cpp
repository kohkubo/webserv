#include "http/request/RequestInfo.hpp"

void RequestInfo::__parse_request_values(const std::string &request_body) {
  tokenVector tokens = tokenize(request_body, "&", "&");
  for (tokenIterator it = tokens.begin(); it != tokens.end(); ++it) {
    // TODO: 今のところ 読み取り文字数を無視して文字列を取り込みしています。
    // TODO: "hoge=huga"の形になっているかのバリデート？
    values_.push_back(*it);
  }
}

// TODO: Content-Typeは何を対応するのか, そもそもパースはするのかわからない
// TODO: chunkedならば先にchenkedパース?
void RequestInfo::parse_request_body(const std::string &request_body) {
  switch (content_type_) {
  case URLENCODED:
    __parse_request_values(request_body);
    break;
  default:
    break;
  }
}
