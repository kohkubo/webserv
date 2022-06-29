#include "http/response/ResponseGenerator.hpp"
#include "utils/file_io_utils.hpp"

#include <fcntl.h>

bool ResponseGenerator::_is_error_status_code(
    HttpStatusCode::StatusCode status_code) {
  // TODO: エラーのステータスコードの扱いを決まったら再実装
  return 399 < status_code && status_code < 600;
}

ResponseGenerator::Body
ResponseGenerator::_open_fd(const std::string &target_path) {
  int  fd = open(target_path.c_str(), O_RDONLY);
  Body body;
  if (fd < 0) {
    ERROR_LOG("open error: " << target_path);
    body.status_code_ = HttpStatusCode::INTERNAL_SERVER_ERROR_500;
  } else {
    body.fd_ = fd;
    // LOG("fd_" << body.fd_);
  }
  return body;
}

ResponseGenerator::Body
ResponseGenerator::_open_write_fd(const std::string &target_path) {
  int fd = open(target_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  //適切な権限(644)はわからない rakiyama
  Body body;
  if (fd < 0) {
    ERROR_LOG("open error: " << target_path);
    body.status_code_ = HttpStatusCode::INTERNAL_SERVER_ERROR_500;
  } else {
    body.fd_ = fd;
  }
  return body;
}
