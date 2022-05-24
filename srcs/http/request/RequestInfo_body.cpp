#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>

void RequestInfo::__parse_request_values(const std::string &request_body) {
  tokenVector tokens = tokenize(request_body, "&", "&");
  for (tokenIterator it = tokens.begin(); it != tokens.end(); ++it) {
    // TODO: 今のところ 読み取り文字数を無視して文字列を取り込みしています。
    // TODO: "hoge=huga"の形になっているかのバリデート必要？
    values_.push_back(*it);
  }
}

// TODO: chunkedならば先にchenkedパースしてからcontent-typeに合わせたパース?
void RequestInfo::parse_request_body(const std::string &request_body) {
  switch (content_type_) {
  case URLENCODED:
    __parse_request_values(request_body);
    break;
  case MULTIPART:
    values_.push_back(request_body); // tmp
    break;
  case PLAIN:
    values_.push_back(request_body); // tmp
    break;
  default:
    // NOTE: htmlを介してpostが送られる場合,
    //       上記3つのどれかになるはず(デフォルトはURLENCODED)
    //       この条件に入ることがあるのかまだわからない
    std::cout << "parse body: unknown content-type" << std::endl; // tmp
    exit(1);
    break;
  }
}
