#include "http/response/ResponseGenerator.hpp"

#include <unistd.h>

#include <algorithm>

#include "http/request/RequestInfo.hpp"
#include "utils/Path.hpp"

namespace response_generator {

static ResponseGenerator::Content
method_get_dir(const RequestInfo &request_info, const Path &target_path) {
  if (!target_path.is_dir_exists()) {
    return create_status_code_content(request_info,
                                      HttpStatusCode::S_404_NOT_FOUND);
  }
  if (!request_info.location_->autoindex_) {
    return create_status_code_content(request_info,
                                      HttpStatusCode::S_403_FORBIDDEN);
  }
  ResponseGenerator::Content content;
  content.action_ = ResponseGenerator::Content::CREATED;
  content.str_    = create_autoindex_body(request_info, target_path);
  return content;
}

static ResponseGenerator::Content
method_get_file(const RequestInfo &request_info, const Path &target_path) {
  if (!target_path.is_file_exists()) {
    ERROR_LOG("method_get_file: file not found");
    return create_status_code_content(request_info,
                                      HttpStatusCode::S_404_NOT_FOUND);
  }
  if (target_path.has_suffix(".py")) {
    Result<ResponseGenerator::Content> result =
        create_cgi_content(request_info, target_path);
    if (result.is_err_) {
      return create_status_code_content(
          request_info, HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
    }
    return result.object_;
  }
  Result<int> result = target_path.open_read_file();
  if (result.is_err_) {
    ERROR_LOG("method_get_file: open file failed");
    return create_status_code_content(
        request_info, HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
  }
  ResponseGenerator::Content content;
  content.action_ = ResponseGenerator::Content::READ;
  content.fd_     = result.object_;
  return content;
}

static ResponseGenerator::Content method_get(const RequestInfo &request_info,
                                             Path              &target_path) {
  if (target_path.has_suffix("/")) {
    Path path_add_index = target_path + request_info.location_->index_;
    if (!path_add_index.is_file_exists()) {
      return method_get_dir(request_info, target_path);
    }
    target_path = path_add_index;
  }
  return method_get_file(request_info, target_path);
}

static ResponseGenerator::Content method_post(const RequestInfo &request_info,
                                              const Path        &target_path) {
  /*
TODO: postでファイル作った場合、content-location 返す必要あるかも？ kohkubo
RFC 9110
9.3.3. POST
If one or more resources has been created on the origin server as a result of
successfully processing a POST request, the origin server SHOULD send a 201
(Created) response containing a Location header field that provides an
identifier for the primary resource created (Section 10.2.2) and a
representation that describes the status of the request while referring to the
new resource(s).
POSTリクエストを正常に処理した結果、
オリジンサーバー上に1つ以上のリソースが作成された場合、
オリジンサーバーは、作成されたプライマリリソースの識別子を提供するLocationヘッダーフィールドを含む201（作成済み）応答を送信する必要があります（セクション10.2）。
.2）および新しいリソースを参照しながらリクエストのステータスを説明する表現。
*/
  if (!request_info.location_->upload_file_ || target_path.has_suffix("/")) {
    return create_status_code_content(request_info,
                                      HttpStatusCode::S_403_FORBIDDEN);
  }
  Result<int> result = target_path.open_write_file();
  if (result.is_err_) {
    ERROR_LOG("method_post: open file failed");
    return create_status_code_content(
        request_info, HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
  }
  ResponseGenerator::Content content;
  content.status_code_ = HttpStatusCode::S_201_CREATED;
  content.action_      = ResponseGenerator::Content::WRITE;
  content.fd_          = result.object_;
  content.str_         = request_info.body_;
  return content;
}

static ResponseGenerator::Content method_delete(const RequestInfo &request_info,
                                                const Path &target_path) {
  HttpStatusCode status_code;
  if (!target_path.is_file_exists()) {
    ERROR_LOG("target file is not found");
    status_code = HttpStatusCode::S_404_NOT_FOUND;
  } else if (!target_path.is_accessible(W_OK)) {
    ERROR_LOG("process can not delete target file");
    status_code = HttpStatusCode::S_403_FORBIDDEN;
  } else if (!target_path.remove_file()) {
    ERROR_LOG("unknown error while deleting file");
    status_code = HttpStatusCode::S_500_INTERNAL_SERVER_ERROR;
  } else {
    ERROR_LOG("deleted file successfully");
    status_code = HttpStatusCode::S_204_NO_CONTENT;
  }
  return create_status_code_content(request_info, status_code);
}

// RFC 9110
// 9.1. Overview
// An origin server that receives a request method that is recognized and
// implemented, but not allowed for the target resource, SHOULD respond with the
// 405 (Method Not Allowed) status code.
static bool is_available_methods(const RequestInfo &request_info) {
  return std::find(request_info.location_->available_methods_.begin(),
                   request_info.location_->available_methods_.end(),
                   request_info.request_line_.method_) !=
         request_info.location_->available_methods_.end();
}

static Path create_default_target_path(const RequestInfo &request_info) {
  return request_info.location_->root_ +
         request_info.request_line_.absolute_path_;
}

ResponseGenerator::Content handle_method(const RequestInfo &request_info) {
  HttpStatusCode status_code;
  Path           target_path = create_default_target_path(request_info);
  if (!is_available_methods(request_info)) {
    status_code = HttpStatusCode::S_405_NOT_ALLOWED;
  } else if ("GET" == request_info.request_line_.method_) {
    return method_get(request_info, target_path);
  } else if ("POST" == request_info.request_line_.method_) {
    return method_post(request_info, target_path);
  } else if ("DELETE" == request_info.request_line_.method_) {
    return method_delete(request_info, target_path);
  } else {
    ERROR_LOG("unknown method: " << request_info.request_line_.method_);
    status_code = HttpStatusCode::S_501_NOT_IMPLEMENTED;
  }
  return create_status_code_content(request_info, status_code);
}

} // namespace response_generator