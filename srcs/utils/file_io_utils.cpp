#include "utils/file_io_utils.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "utils/syscall_wrapper.hpp"
#include "utils/utils.hpp"

bool is_file_exists(const std::string &path) {
  struct stat file_info = {};

  if (stat(path.c_str(), &file_info) == -1) {
    ERROR_LOG("error: is_file_exists: " << path);
    return false;
  }
  return ((file_info.st_mode & S_IFMT) == S_IFREG);
}

// 末尾が"/"でなくてもディレクトリとして扱われることに注意
bool is_dir_exists(const std::string &path) {
  struct stat file_info = {};

  if (stat(path.c_str(), &file_info) == -1) {
    ERROR_LOG("error: is_dir_exists: " << path);
    return false;
  }
  return ((file_info.st_mode & S_IFMT) == S_IFDIR);
}

RetPair read_file_to_str(const std::string &path) {
  RetPair       ret_pair = {};
  std::ifstream file(path.c_str());
  if (file.fail()) {
    ret_pair.is_err_ = true;
    return ret_pair;
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  // 空ファイルの時: buffer.fail() -> true, buffer.str() = "": 問題なし.
  // その他のエラーケースに関しても再現が困難なので現状エラー確認なし.
  file.close();
  ret_pair.str_ = buffer.str();
  return ret_pair;
}

bool remove_file(const std::string &file_path) {
  int ret = std::remove(file_path.c_str());
  if (ret == -1) {
    ERROR_LOG_WITH_ERRNO("remove_file");
    return false;
  }
  return true;
}

bool is_accessible(const std::string &file_path, int mode) {
  return access(file_path.c_str(), mode) == 0;
}
