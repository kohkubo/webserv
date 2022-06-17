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

std::string read_file_tostring(const std::string &path) {
  std::ifstream file(path.c_str());
  if (file.fail()) {
    ERROR_LOG_WITH_ERRNO("read_file_tostring:" + path);
    return ""; // TODO: エラーを呼び出し元に通知 rakiyama
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  // 空ファイルの時: buffer.fail() -> true, buffer.str() = "": 問題なし.
  // その他のエラーケースに関しても再現が困難なので現状エラー確認なし.
  file.close();
  return buffer.str();
}

// ファイルが既に存在する時の挙動まだわからない rakiyama
bool write_string_tofile(const std::string &file_path,
                         const std::string &content) {
  std::ofstream outfile(file_path.c_str());
  if (outfile.fail()) {
    ERROR_EXIT_WITH_ERRNO("write_string_tofile");
    return false;
  }
  outfile << content;
  outfile.close();
  return true;
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
