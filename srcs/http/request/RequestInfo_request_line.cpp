#include "http/request/RequestInfo.hpp"

#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

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
  if (request_line_.absolute_path_.is_minus_depth()) {
    throw BadRequestException();
  }
  request_line_.http_version_ = request_line.substr(last_sp + 1);
}
