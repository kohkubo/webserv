#include "http/response/ResponseGenerator.hpp"

#include <unistd.h>

#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"

// TODO: リンクやその他のファイルシステムの時どうするか
static HttpStatusCode check_filepath_status(const Location    &location,
                                            const std::string &file_path) {
  if (has_suffix(file_path, "/")) {
    if (is_dir_exists(file_path)) {
      if (!location.autoindex_) {
        return FORBIDDEN_403; // nginxに合わせた
      }
      return OK_200;
    }
    return NOT_FOUND_404;
  }
  if (!is_file_exists(file_path)) {
    return NOT_FOUND_404;
  }
  if (!is_accessible(file_path, R_OK)) {
    // TODO: Permission error が 403なのか確かめてない
    return FORBIDDEN_403;
  }
  return OK_200;
}

static bool save_files(const RequestInfo::formMap &form_map,
                       const std::string          &target_dir) {
  bool                                       res = true;
  typedef std::map<std::string, std::string> paramMap;

  RequestInfo::formMap::const_iterator       it = form_map.begin();
  for (; it != form_map.end(); it++) {
    const RequestInfo::Form &form        = it->second;
    const paramMap          &param_map   = form.content_disposition_.parameter_;
    paramMap::const_iterator it_filename = param_map.find("filename");
    if (it_filename != param_map.end()) {
      // ひとまずfilenameがある時のみ処理 rakiyama
      const std::string &filename       = it_filename->second;
      std::string        save_file_path = target_dir + filename;
      if (is_file_exists(save_file_path)) {
        continue; // tmp
      }
      if (!write_string_tofile(save_file_path, form.content_)) {
        res = false;
      }
    }
  }
  return res;
}

// is_minus_depthは考慮してない rakiyama
static HttpStatusCode save_posted_file(const Location    &location,
                                       const RequestInfo &request_info,
                                       const std::string &target_path) {
  if (!location.upload_file_) {
    return NOT_ALLOWED_405;
  }
  if (request_info.content_type_.type_ != "multipart/form-data") {
    // text/plainでのPOSTに対応する場合は同じようなデータ構造で持ちたい
    // rakiyama
    return INTERNAL_SERVER_ERROR_500;
  }
  if (!has_suffix(target_path, "/")) {
    // file_pathにindexがappendされていたらここで引っかかることに注意
    // indexをつける作業はGETに移せば解決すると踏んでいる
    // rakiyama
    return INTERNAL_SERVER_ERROR_500;
  }
  if (!is_dir_exists(target_path)) {
    return NOT_FOUND_404;
  }
  if (!is_accessible(target_path, W_OK)) {
    // W_OKが適切かは調べてない rakiyama
    return FORBIDDEN_403;
  }
  if (!save_files(request_info.form_map_, target_path)) {
    return INTERNAL_SERVER_ERROR_500;
  }
  return Created_201;
}

static HttpStatusCode handle_post_method(const Location    &location,
                                         const RequestInfo &request_info,
                                         const std::string &file_path) {
  // TODO: ここらへんの処理、未定なので雑に書いています。
  if (std::find(location.limit_except_.begin(), location.limit_except_.end(),
                "POST") == location.limit_except_.end()) {
    return NOT_ALLOWED_405;
  }
  return save_posted_file(location, request_info, file_path);
}

static HttpStatusCode handle_get_method(const Location    &location,
                                        const std::string &file_path) {
  // TODO: ここらへんの処理、未定なので雑に書いています。
  if (std::find(location.limit_except_.begin(), location.limit_except_.end(),
                "GET") == location.limit_except_.end()) {
    return NOT_ALLOWED_405;
  }
  return check_filepath_status(location, file_path);
}

static HttpStatusCode delete_target_file(const RequestInfo &request_info,
                                         const std::string &file_path) {
  if (request_info.content_length_ != 0) {
    ERROR_LOG("DELETE with body is unsupported");
    return BAD_REQUEST_400;
  }
  if (!is_file_exists(file_path)) {
    ERROR_LOG("target file is not found");
    return NOT_FOUND_404;
  }
  if (!is_accessible(file_path, W_OK)) {
    ERROR_LOG("process can not delete target file");
    return FORBIDDEN_403;
  }
  if (!remove_file(file_path)) {
    ERROR_LOG("unknown error while deleting file");
    return INTERNAL_SERVER_ERROR_500;
  }
  ERROR_LOG("deleted file successfully");
  return NO_CONTENT_204;
}

static HttpStatusCode handle_delete_method(const Location    &location,
                                           const RequestInfo &request_info,
                                           const std::string &file_path) {
  // TODO: ここらへんの処理、未定なので雑に書いています。
  if (std::find(location.limit_except_.begin(), location.limit_except_.end(),
                "DELETE") == location.limit_except_.end()) {
    return NOT_ALLOWED_405;
  }
  return delete_target_file(request_info, file_path);
}

HttpStatusCode
ResponseGenerator::_handle_method(const Location    &location,
                                  const RequestInfo &request_info,
                                  const std::string &file_path) {
  HttpStatusCode status_code = NONE;
  if ("GET" == request_info.method_) {
    status_code = handle_get_method(location, file_path);
  } else if ("POST" == request_info.method_) {
    status_code = handle_post_method(location, request_info, file_path);
  } else if ("DELETE" == request_info.method_) {
    status_code = handle_delete_method(location, request_info, file_path);
  } else {
    LOG("unknown method: " << request_info.method_);
    status_code = NOT_IMPLEMENTED_501;
  }
  return status_code;
}