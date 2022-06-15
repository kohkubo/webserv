#include "http/response/ResponseGenerator.hpp"

#include <unistd.h>

#include <iostream>
#include <map>

#include "config/Config.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

// is_minus_depthは考慮してない
HttpStatusCode
ResponseGenerator::__save_posted_file(const Location    &location,
                                      const RequestInfo &request_info,
                                      const std::string &file_path) {
  std::cout << "~~~~~~" << std::endl;
  if (!location.upload_file_) {
    return NOT_ALLOWED_405;
  }
  if (request_info.content_type_.type_ != "multipart/form-data") {
    return INTERNAL_SERVER_ERROR_500;
  }
  if (!has_suffix(file_path, "/")) {
    // rakiyama
    // indexがappendされていたらここで引っかかることに注意
    // ディレクトリとして指定されているの判断方法はsuffixで統一
    return INTERNAL_SERVER_ERROR_500;
  }
  if (!is_dir_exists(file_path)) {
    return NOT_FOUND_404;
  }
  if (!is_accessible(file_path, W_OK)) {
    // W_OKが適切かは調べてない rakiyama
    return FORBIDDEN_403;
  }
  RequestInfo::formMap::const_iterator it = request_info.form_map_.begin();
  for (; it != request_info.form_map_.end(); it++) {
    std::map<std::string, std::string>::const_iterator itf =
        it->second.content_disposition_.parameter_.find("filename");
    if (itf != it->second.content_disposition_.parameter_.end()) {
      // とりあえずfilenameがある時のみ処理 rakiyama
      std::string save_file_path = file_path + itf->second;
      if (!is_file_exists(save_file_path)) {
        // とりあえず存在しない時だけ保存処理 rakiyama
        if (!write_string_tofile(save_file_path, it->second.content_))
          return INTERNAL_SERVER_ERROR_500;
        // loopが全て終わったらこれ返す rakiyama
      }
    }
  }
  return Created_201;
}
