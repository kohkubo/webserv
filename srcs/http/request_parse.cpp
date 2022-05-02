#include "config/ServerConfig.hpp"
#include "util/tokenize.hpp"
#include "util/util.hpp"
#include "HttpMessage.hpp"
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

void parse_request_method_line(HttpMessage              &request_message,
                               std::vector<std::string> &request_tokens) {
  request_message.method_  = parse_request_method(request_tokens[0]); // "GET"
  request_message.url_     = request_tokens[1]; // "example.com/test/test"
  request_message.version_ = request_tokens[2]; // "HTTP/1.1"
}

void parse_request_host(HttpMessage              &request_message,
                        std::vector<std::string> &request_tokens) {
  std::vector<std::string>::iterator it_host =
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

void parse_request_content_length(HttpMessage              &request_message,
                                  std::vector<std::string> &request_tokens) {
  std::vector<std::string>::iterator it_content_len = std::find(
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
void parse_request_body(HttpMessage              &request_message,
                        std::vector<std::string> &request_tokens) {
  if (request_message.method_ != POST || request_message.content_length_ == 0) {
    return;
  }
  for (std::vector<std::string>::iterator it = request_tokens.begin();
       it != request_tokens.end(); ++it) {
    if (*it == "\r" && *(it + 1) == "\n" && *(it + 2) == "\r" &&
        *(it + 3) == "\n") {
      std::vector<std::string> request_values = tokenize(*(it + 4), "&", "&");
      for (std::vector<std::string>::iterator it_value = request_values.begin();
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