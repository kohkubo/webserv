#include "http/response/ResponseGenerator.hpp"

#include <unistd.h>

#include <algorithm>
#include <iostream>

#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"

// TODO: リンクやその他のファイルシステムの時どうするか
static HttpStatusCode::StatusCode
check_filepath_status(const RequestInfo &request_info) {
  if (has_suffix(request_info.target_path_, "/")) {
    if (is_dir_exists(request_info.target_path_)) {
      if (!request_info.location_->autoindex_) {
        return HttpStatusCode::FORBIDDEN_403;
      }
      return HttpStatusCode::OK_200;
    }
    return HttpStatusCode::NOT_FOUND_404;
  }
  if (!is_file_exists(request_info.target_path_)) {
    ERROR_LOG("check_filepath_status: file not found");
    return HttpStatusCode::NOT_FOUND_404;
  }
  if (!is_accessible(request_info.target_path_, R_OK)) {
    // TODO: Permission error が 403なのか確かめてない
    return HttpStatusCode::FORBIDDEN_403;
  }
  return HttpStatusCode::OK_200;
}

static HttpStatusCode::StatusCode
delete_target_file(const RequestInfo &request_info) {
  if (!is_file_exists(request_info.target_path_)) {
    ERROR_LOG("target file is not found");
    return HttpStatusCode::NOT_FOUND_404;
  }
  if (!is_accessible(request_info.target_path_, W_OK)) {
    ERROR_LOG("process can not delete target file");
    return HttpStatusCode::FORBIDDEN_403;
  }
  if (!remove_file(request_info.target_path_)) {
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
                   request_info.request_line_.method_) ==
         request_info.location_->available_methods_.end();
}

HttpStatusCode::StatusCode
ResponseGenerator::_handle_method(const RequestInfo &request_info) {
  HttpStatusCode::StatusCode status_code = HttpStatusCode::NONE;
  // TODO: 501が必要？ kohkubo
  /*
  RFC 9110
  9.1. Overview
  An origin server that receives a request method that is unrecognized or not
  implemented SHOULD respond with the 501 (Not Implemented) status code.
  認識されないか実装されていないリクエストメソッドを受信するオリジンサーバーは、501（実装されていない）ステータスコードで応答する必要があります。
  */
  if (is_available_methods(request_info)) {
    return HttpStatusCode::NOT_ALLOWED_405;
  }
  if ("GET" == request_info.request_line_.method_) {
    status_code = check_filepath_status(request_info);
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

TODO: postでbodyがない場合もエラー？ kohkubo
RFC 9110
8.6. Content-Length

A user agent SHOULD send Content-Length in a request when the method defines a
meaning for enclosed content and it is not sending Transfer-Encoding. >
メソッドが囲まれたコンテンツの意味を定義し、Transfer-Encodingを送信していない場合、ユーザーエージェントはリクエストでContent-Lengthを送信する必要があります。

For example, a user agent normally sends Content- Length in a POST request
even when the value is 0 (indicating empty content). >
たとえば、ユーザーエージェントは通常、値が0（空のコンテンツを示す）の場合でも、POSTリクエストでContent-Lengthを送信します。
*/
    status_code = check_filepath_status(request_info);
  } else if ("DELETE" == request_info.request_line_.method_) {
    status_code = delete_target_file(request_info);
  } else {
    ERROR_LOG("unknown method: " << request_info.request_line_.method_);
    status_code = HttpStatusCode::NOT_IMPLEMENTED_501;
  }
  return status_code;
}