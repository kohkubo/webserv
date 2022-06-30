#include "http/response/ResponseGenerator.hpp"

#include <unistd.h>

#include <algorithm>
#include <iostream>

#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"

// TODO: リンクやその他のファイルシステムの時どうするか
static HttpStatusCode check_filepath_status(const RequestInfo &request_info,
                                            const std::string &target_path) {
  if (has_suffix(target_path, "/")) {
    if (is_dir_exists(target_path)) {
      if (!request_info.location_->autoindex_) {
        return HttpStatusCode::S_403_FORBIDDEN;
      }
      return HttpStatusCode::S_200_OK;
    }
    return HttpStatusCode::S_404_NOT_FOUND;
  }
  if (!is_file_exists(target_path)) {
    ERROR_LOG("check_filepath_status: file not found");
    return HttpStatusCode::S_404_NOT_FOUND;
  }
  return HttpStatusCode::S_200_OK;
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

static std::string create_target_path(const RequestInfo &request_info) {
  return request_info.location_->root_ +
         request_info.request_line_.absolute_path_;
}

HttpStatusCode ResponseGenerator::_method_get(const RequestInfo &request_info,
                                              std::string        target_path) {
  HttpStatusCode status_code;
  if (has_suffix(target_path, "/") &&
      is_file_exists(target_path + request_info.location_->index_)) {
    target_path += request_info.location_->index_;
  }
  status_code = check_filepath_status(request_info, target_path);
  if (status_code.is_error_status_code()) {
    return status_code;
  }
  if (has_suffix(target_path, ".py")) {
    Result<std::string> result =
        _read_file_to_str_cgi(request_info, target_path);
    if (!result.is_err_) {
      _body_.content_     = result.object_;
      _body_.has_content_ = true;
      return status_code;
    }
    return HttpStatusCode::S_500_INTERNAL_SERVER_ERROR;
  }
  if (has_suffix(target_path, "/")) {
    _body_.content_     = _create_autoindex_body(request_info, target_path);
    _body_.has_content_ = true;
    return status_code;
  }
  Result<int> result = open_read_file(target_path);
  if (result.is_err_) {
    return HttpStatusCode::S_500_INTERNAL_SERVER_ERROR;
  }
  _body_.action_ = Body::READ;
  _body_.fd_     = result.object_;

  return status_code;
}

HttpStatusCode ResponseGenerator::_method_post(const RequestInfo &request_info,
                                               std::string        target_path) {
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
  if (!request_info.location_->upload_file_ || has_suffix(target_path, "/")) {
    return HttpStatusCode::S_403_FORBIDDEN;
  }
  Result<int> result = open_write_file(target_path);
  if (result.is_err_) {
    return HttpStatusCode::S_500_INTERNAL_SERVER_ERROR;
  }
  _body_.action_      = Body::WRITE;
  _body_.fd_          = result.object_;
  _body_.content_     = request_info.body_;
  _body_.has_content_ = true;

  return HttpStatusCode::S_201_CREATED;
}

HttpStatusCode
ResponseGenerator::_method_delete(const std::string &target_path) {
  if (!is_file_exists(target_path)) {
    ERROR_LOG("target file is not found");
    return HttpStatusCode::S_404_NOT_FOUND;
  }
  if (!is_accessible(target_path, W_OK)) {
    ERROR_LOG("process can not delete target file");
    return HttpStatusCode::S_403_FORBIDDEN;
  }
  if (!remove_file(target_path)) {
    ERROR_LOG("unknown error while deleting file");
    return HttpStatusCode::S_500_INTERNAL_SERVER_ERROR;
  }
  ERROR_LOG("deleted file successfully");
  return HttpStatusCode::S_204_NO_CONTENT;
}

HttpStatusCode
ResponseGenerator::_handle_method(const RequestInfo &request_info) {
  // TODO: 501が必要？ kohkubo
  /*
  RFC 9110
  9.1. Overview
  An origin server that receives a request method that is unrecognized or not
  implemented SHOULD respond with the 501 (Not Implemented) status code.
  認識されないか実装されていないリクエストメソッドを受信するオリジンサーバーは、501（実装されていない）ステータスコードで応答する必要があります。
  */
  if (!is_available_methods(request_info)) {
    return HttpStatusCode::S_405_NOT_ALLOWED;
  }
  std::string target_path = create_target_path(request_info);
  if ("GET" == request_info.request_line_.method_) {
    return _method_get(request_info, target_path);
  }
  if ("POST" == request_info.request_line_.method_) {
    return _method_post(request_info, target_path);
  }
  if ("DELETE" == request_info.request_line_.method_) {
    return _method_delete(target_path);
  }
  ERROR_LOG("unknown method: " << request_info.request_line_.method_);
  return HttpStatusCode::S_501_NOT_IMPLEMENTED;
}