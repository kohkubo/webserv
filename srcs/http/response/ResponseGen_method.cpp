#include "http/response/ResponseGenerator.hpp"

#include <unistd.h>

#include <algorithm>
#include <iostream>

#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"

// TODO: リンクやその他のファイルシステムの時どうするか
static HttpStatusCode::StatusCode
check_filepath_status(const RequestInfo &request_info,
                      const std::string &target_path) {
  if (has_suffix(target_path, "/")) {
    if (is_dir_exists(target_path)) {
      if (!request_info.location_->autoindex_) {
        return HttpStatusCode::FORBIDDEN_403;
      }
      return HttpStatusCode::OK_200;
    }
    return HttpStatusCode::NOT_FOUND_404;
  }
  if (!is_file_exists(target_path)) {
    ERROR_LOG("check_filepath_status: file not found");
    return HttpStatusCode::NOT_FOUND_404;
  }
  return HttpStatusCode::OK_200;
}

// ファイルが既に存在 -> 上書き
// ファイルパス中のディレクトリがない -> ファイル作成過程で500
static HttpStatusCode::StatusCode
check_postfile_status(const RequestInfo &request_info,
                      const std::string &target_path) {
  if (!request_info.location_->upload_file_ || has_suffix(target_path, "/")) {
    return HttpStatusCode::FORBIDDEN_403;
  }
  return HttpStatusCode::Created_201;
}

// ファイルが既に存在 -> 上書き
// ファイルパス中のディレクトリがない -> ファイル作成過程で500
static HttpStatusCode::StatusCode
check_postfile_status(const RequestInfo &request_info,
                      const std::string &target_path) {
  if (!request_info.location_->upload_file_ || has_suffix(target_path, "/")) {
    return HttpStatusCode::FORBIDDEN_403;
  }
  return HttpStatusCode::Created_201;
}

static HttpStatusCode::StatusCode
delete_target_file(const std::string &target_path) {
  if (!is_file_exists(target_path)) {
    ERROR_LOG("target file is not found");
    return HttpStatusCode::NOT_FOUND_404;
  }
  if (!is_accessible(target_path, W_OK)) {
    ERROR_LOG("process can not delete target file");
    return HttpStatusCode::FORBIDDEN_403;
  }
  if (!remove_file(target_path)) {
    ERROR_LOG("unknown error while deleting file");
    return HttpStatusCode::INTERNAL_SERVER_ERROR_500;
  }
  ERROR_LOG("deleted file successfully");
  return HttpStatusCode::NO_CONTENT_204;
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

ResponseGenerator::Body
ResponseGenerator::_handle_method(const RequestInfo &request_info) {
  Body body;
  // TODO: 501が必要？ kohkubo
  /*
  RFC 9110
  9.1. Overview
  An origin server that receives a request method that is unrecognized or not
  implemented SHOULD respond with the 501 (Not Implemented) status code.
  認識されないか実装されていないリクエストメソッドを受信するオリジンサーバーは、501（実装されていない）ステータスコードで応答する必要があります。
  */
  std::string target_path =
      request_info.location_->root_ + request_info.request_line_.absolute_path_;
  if (!is_available_methods(request_info)) {
    body.status_code_ = HttpStatusCode::NOT_ALLOWED_405;
  } else if ("GET" == request_info.request_line_.method_) {
    if (has_suffix(target_path, "/") &&
        is_file_exists(target_path + request_info.location_->index_)) {
      target_path += request_info.location_->index_;
    }
    body.status_code_ = check_filepath_status(request_info, target_path);
    if (_is_error_status_code(body.status_code_)) {
      return body;
    }
    if (has_suffix(target_path, ".py")) {
      Result<std::string> result =
          _read_file_to_str_cgi(request_info, target_path);
      if (!result.is_err_) {
        body.content_     = result.object_;
        body.has_content_ = true;
        return body;
      }
      body.status_code_ = HttpStatusCode::INTERNAL_SERVER_ERROR_500;
      return body;
    }
    if (has_suffix(target_path, "/")) {
      body.content_     = _create_autoindex_body(request_info, target_path);
      body.has_content_ = true;
      return body;
    }
    body = _open_read_fd(target_path);
  } else if ("POST" == request_info.request_line_.method_) {
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
    body.status_code_ = check_postfile_status(request_info, target_path);
    if (_is_error_status_code(body.status_code_)) {
      return body;
    }
    body              = _open_write_fd(body, target_path);
    body.content_     = request_info.body_;
    body.has_content_ = true;
    return body;
  } else if ("DELETE" == request_info.request_line_.method_) {
    body.status_code_ = delete_target_file(target_path);
  } else {
    ERROR_LOG("unknown method: " << request_info.request_line_.method_);
    body.status_code_ = HttpStatusCode::NOT_IMPLEMENTED_501;
  }
  return body;
}