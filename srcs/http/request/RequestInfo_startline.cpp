#include "http/request/RequestInfo.hpp"

#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

// 堅牢性のために、要求行を受信して​​解析することを期待しているサーバーは、
// 要求行の前に受信された少なくとも1つの空行（CRLF）を無視する必要があります（SHOULD）。
// rfc 7230 3.5

bool RequestInfo::has_request_line(std::string &request_buffer) {
  if (__is_first_line_ && has_prefix(request_buffer, CRLF)) {
    __cut_buffer(request_buffer, CRLF.size());
    __is_first_line_ = false;
  }
  std::size_t pos = request_buffer.find(CRLF);
  if (pos == std::string::npos) {
    return false;
  }
  return true;
}

std::string RequestInfo::cut_request_line(std::string &request_buffer) {
  std::size_t pos = request_buffer.find(CRLF);
  std::string res = request_buffer.substr(0, pos);
  request_buffer  = request_buffer.substr(pos + CRLF.size());
  return res;
}

// request-line = method SP request-target SP HTTP-version (CRLF)
void RequestInfo::parse_request_start_line(const std::string &request_line) {
  std::size_t first_sp = request_line.find_first_of(' ');
  std::size_t last_sp  = request_line.find_last_of(' ');
  if (first_sp == std::string::npos || first_sp == last_sp) {
    throw BadRequestException();
  }
  std::string method_str = request_line.substr(0, first_sp);
  method_                = __parse_request_method(method_str);
  uri_     = request_line.substr(first_sp + 1, last_sp - (first_sp + 1));
  version_ = request_line.substr(last_sp + 1);
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
