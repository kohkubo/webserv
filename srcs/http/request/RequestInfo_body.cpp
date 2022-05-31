#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>

#include "utils/utils.hpp"

// TODO: chunkedならば先にchenkedパースしてからcontent-typeに合わせたパースかも
// TODO: content-typeの文法を確認する -> headerのパースで確認すべきかもしれない
void RequestInfo::parse_request_body(std::string       &request_body,
                                     const std::string &content_type) {
  if (content_type == "application/x-www-form-urlencoded") {
    __parse_request_values(request_body);
    return;
  }
  if (content_type == "multipart/form-data") {
    env_values_.push_back(request_body); // tmp
    return;
  }
  if (content_type == "text/plain") {
    env_values_.push_back(request_body); // tmp
    return;
  }
  // htmlを介してpostが送られる場合,
  // 上記3つのどれかになるはず(デフォルトは"application/x-www-form-urlencoded")
  // それ以外になることがあるのかまだわからない
  std::cerr << "parse body: unknown content-type" << std::endl; // tmp
  exit(EXIT_FAILURE);
}

void RequestInfo::__parse_request_values(const std::string &request_body) {
  tokenVector tokens = tokenize(request_body, "&", "&");
  for (tokenIterator it = tokens.begin(); it != tokens.end(); ++it) {
    // TODO: 今のところ 読み取り文字数を無視して文字列を取り込みしています。
    // TODO: "hoge=huga"の形になっているかのバリデート必要かも
    env_values_.push_back(*it);
  }
}
