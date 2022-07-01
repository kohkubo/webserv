#include "http/request/RequestInfo.hpp"

#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

// 堅牢性のために、要求行を受信して​​解析することを期待しているサーバーは、
// 要求行の前に受信された少なくとも1つの空行（CRLF）を無視する必要があります（SHOULD）。
// rfc 7230 3.5

// TODO: 2022/06/25 kohkubo
// RFC 9110
// 4.2.3.http(s) Normalization and Comparison
// 以下は同じ扱い
/*
      http://example.com:80/~smith/home.html
      http://EXAMPLE.com/%7Esmith/home.html
      http://EXAMPLE.com:/%7esmith/home.html
*/

void RequestInfo::parse_request_line(const std::string &request_line) {
  std::size_t first_sp = request_line.find_first_of(' ');
  std::size_t last_sp  = request_line.find_last_of(' ');
  if (first_sp == std::string::npos || first_sp == last_sp) {
    throw BadRequestException();
  }
  request_line_.method_ = request_line.substr(0, first_sp);
  std::string tmp_path =
      request_line.substr(first_sp + 1, last_sp - (first_sp + 1));
  std::size_t query_pos = tmp_path.find('?');
  if (query_pos != std::string::npos) {
    request_line_.query_         = tmp_path.substr(query_pos + 1);
    request_line_.absolute_path_ = tmp_path.erase(query_pos);
  } else {
    request_line_.absolute_path_ = tmp_path;
  }
  request_line_.http_version_ = request_line.substr(last_sp + 1);
}
