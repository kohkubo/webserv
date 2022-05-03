#include "HttpMessage.hpp"
#include "config/ServerConfig.hpp"
#include "util/tokenize.hpp"
#include "util/util.hpp"
#include <algorithm>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

static HttpMethod parse_request_method(const std::string &method) {
  if (method == "GET") {
    return GET;
  }
  if (method == "POST") {
    return POST;
  }
  if (method == "DELETE") {
    return DELETE;
  }
  return UNKNOWN;
}

static void parse_request_method_line(HttpMessage  &request_message,
                                      token_vector &request_tokens) {
  request_message.method_  = parse_request_method(request_tokens[0]); // "GET"
  request_message.url_     = request_tokens[1]; // "example.com/test/test"
  request_message.version_ = request_tokens[2]; // "HTTP/1.1"
}

static void parse_request_host(HttpMessage  &request_message,
                               token_vector &request_tokens) {
  token_iterator it_host =
      std::find(request_tokens.begin(), request_tokens.end(), "Host:");
  if (it_host != request_tokens.end()) {
    std::string            request_line_host = *(it_host + 1);
    std::string::size_type pos               = request_line_host.find(':');
    if (pos == std::string::npos) {
      request_message.host_ = request_line_host;
      return;
    }
    request_message.host_ = request_line_host.substr(0, pos);
  }
}

static void parse_request_content_length(HttpMessage  &request_message,
                                         token_vector &request_tokens) {
  token_iterator it_content_len = std::find(
      request_tokens.begin(), request_tokens.end(), "Content-Length:");
  if (it_content_len != request_tokens.end()) {
    // TODO: 98対応に書き換え必要
    request_message.content_length_ = atol((*(it_content_len + 1)).c_str());
  }
}

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
static void parse_request_body(HttpMessage  &request_message,
                               token_vector &request_tokens) {
  if (request_message.method_ != POST || request_message.content_length_ == 0) {
    return;
  }
  for (token_iterator it = request_tokens.begin(); it != request_tokens.end();
       ++it) {
    if (*it == "\r" && *(it + 1) == "\n" && *(it + 2) == "\r" &&
        *(it + 3) == "\n") {
      token_vector request_values = tokenize(*(it + 4), "&", "&");
      for (token_iterator it_value = request_values.begin();
           it_value != request_values.end(); ++it_value) {
        // TODO: 今のところ 読み取り文字数を無視して文字列を取り込みしています。
        std::pair<std::string, std::string> value =
            parse_request_value(*it_value);
        request_message.values_[value.first] = value.second;
      }
      break;
    }
  }
}

/*
 * 超安易パース
 * 必須情報のみを取得しています。
 */
HttpMessage parse_request_message(token_vector &request_tokens) {
  HttpMessage request_message;
  parse_request_method_line(request_message, request_tokens);
  parse_request_host(request_message, request_tokens);
  parse_request_content_length(request_message, request_tokens);
  parse_request_body(request_message, request_tokens);
  return request_message;
}
