#include "http/request/RequestInfo.hpp"

#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

// 堅牢性のために、要求行を受信して​​解析することを期待しているサーバーは、
// 要求行の前に受信された少なくとも1つの空行（CRLF）を無視する必要があります（SHOULD）。
// rfc 7230 3.5

// request-line = method SP request-target SP HTTP-version (CRLF)
bool RequestInfo::parse_request_start_line(const std::string &request_line) {
  if (__is_first_line_ && request_line == "") {
    __is_first_line_ = false;
    return false;
  }
  std::size_t first_sp = request_line.find_first_of(' ');
  std::size_t last_sp  = request_line.find_last_of(' ');
  if (first_sp == std::string::npos || first_sp == last_sp) {
    throw BadRequestException();
  }
  std::string method_str = request_line.substr(0, first_sp);
  method_                = __parse_request_method(method_str);
  uri_     = request_line.substr(first_sp + 1, last_sp - (first_sp + 1));
  version_ = request_line.substr(last_sp + 1);
  return true;
}

HttpMethod RequestInfo::__parse_request_method(const std::string &method) {
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
