#include "http/response/ResponseGenerator.hpp"

#include <cstdlib>
#include <unistd.h>

#include <iostream>
#include <map>
#include <stdexcept>

#include "config/Config.hpp"
#include "config/Location.hpp"
#include "event/Response.hpp"
#include "http/HttpStatusCode.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

namespace response_generator {

ResponseGenerator::ResponseGenerator(const RequestInfo &request_info,
                                     HttpStatusCode     status_code)
    : _request_info_(request_info)
    , _is_connection_close_(_request_info_.connection_close_ ||
                            status_code == HttpStatusCode::C_400_BAD_REQUEST ||
                            status_code ==
                                HttpStatusCode::S_413_ENTITY_TOO_LARGE) {
  if (status_code.is_error_status_code()) {
    content_ = create_status_code_content(_request_info_, status_code);
    return;
  }
  if (_request_info_.location_ == NULL) {
    content_ = create_status_code_content(_request_info_,
                                          HttpStatusCode::S_404_NOT_FOUND);
    return;
  }
  if (_request_info_.location_->return_map_.size() != 0) {
    status_code = _request_info_.location_->return_map_.begin()->first;
    content_    = create_status_code_content(_request_info_, status_code);
    return;
  }
  content_ = handle_method(_request_info_);
}

Response ResponseGenerator::generate_response() {
  if (content_.state_ == Content::READ) {
    return Response("", _is_connection_close_, Response::READING);
  }
  if (content_.state_ == Content::WRITE) {
    if (write(content_.fd_, content_.str_.c_str(), content_.str_.size()) ==
        -1) {
      content_ = create_status_code_content(
          _request_info_, HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
    }
    content_ = create_status_code_content(_request_info_,
                                          HttpStatusCode::S_201_CREATED);
    content_.fd_ =
        -1; // writeが成功してもfd=-1にして_create_status_code_body()を使うようにする
  }
  // if (content_.state_ == Content::WRITE) {
  //   return Response("", _is_connection_close_, Response::WRITING);
  // }
  // CREATEのとき
  return Response(create_response_message(content_.str_),
                  _is_connection_close_);
}

} // namespace response_generator