#include "http/response/Response.hpp"

#include <dirent.h>

#include "utils/syscall_wrapper.hpp"

// directory内のファイル・ディレクトリ名をstringに格納して返す
// TODO: htmlに変換する
std::string Response::__create_autoindex_body(const std::string &file_path) {
  DIR                     *dir = xopendir(file_path.c_str());
  struct dirent           *diread;
  std::vector<std::string> files;

  // xopendir()が成功している時点でdirは有効なものなので
  // 他のシステムコールはエラーを起こさないことが前提の実装
  while ((diread = readdir(dir)) != NULL) {
    files.push_back(diread->d_name);
  }
  closedir(dir);

  std::string                        ret;
  std::vector<std::string>::iterator it = files.begin();
  for (; it != files.end(); it++) {
    ret += " " + *it;
  }

  return ret;
}
