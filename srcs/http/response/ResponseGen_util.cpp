#include "http/response/ResponseGenerator.hpp"
#include "utils/file_io_utils.hpp"

#include <fcntl.h>

bool ResponseGenerator::_is_error_status_code(
    HttpStatusCode::StatusCode status_code) {
  // TODO: エラーのステータスコードの扱いを決まったら再実装
  return 399 < status_code && status_code < 600;
}
