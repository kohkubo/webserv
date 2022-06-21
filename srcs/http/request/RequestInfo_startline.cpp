#include "http/request/RequestInfo.hpp"

#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

// 堅牢性のために、要求行を受信して​​解析することを期待しているサーバーは、
// 要求行の前に受信された少なくとも1つの空行（CRLF）を無視する必要があります（SHOULD）。
// rfc 7230 3.5

// request-line = method SP request-target SP HTTP-version (CRLF)
void RequestInfo::parse_request_start_line(const std::string &request_line) {
  std::size_t first_sp = request_line.find_first_of(' ');
  std::size_t last_sp  = request_line.find_last_of(' ');
  method_              = request_line.substr(0, first_sp);
  request_target_ = request_line.substr(first_sp + 1, last_sp - (first_sp + 1));
  std::size_t query_pos = request_target_.find('?');
  if (query_pos != std::string::npos) {
    query_string_ = request_target_.substr(query_pos + 1);
    // request_target_からquery_stringを削除しているので注意（いっしょに持った方がいいかも）
    request_target_.erase(query_pos);
  }
  version_ = request_line.substr(last_sp + 1);
}

void RequestInfo::check_bad_parse_request_start_line(
    const std::string &request_line) {
  std::size_t first_sp = request_line.find_first_of(' ');
  std::size_t last_sp  = request_line.find_last_of(' ');
  if (first_sp == std::string::npos || first_sp == last_sp) {
    throw BadRequestException();
  }
}
