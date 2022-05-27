#include "utils/file_io_utils.hpp"

#include <dirent.h>
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

bool is_path_exists(const std::string &path) {
  struct stat file_info;

  if (stat(path.c_str(), &file_info) == -1) {
    std::cout << "error: is_path_exists: " << path << std::endl;
    return false;
  }
  // TODO: POSTの時はS_IFDIR(ディレクトリ)を許容するか
  switch ((file_info.st_mode & S_IFMT)) {
  case S_IFREG:
    return true;
  case S_IFDIR:
    return true;
  default:
    return false;
  }
}

// TODO: htmlに変換する
std::string read_dir_tostring(const std::string &file_path) {
  DIR                     *dir = xopendir(file_path.c_str());
  struct dirent           *diread;
  std::vector<std::string> files;

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

std::string read_file_tostring(const std::string &path) {
  std::ifstream file(path.c_str());
  if (file.fail()) {
    std::cout << path << " is not found." << std::endl;
    std::cout << "error: read_file_tostring" << std::endl;
    return ""; // TODO: エラーを呼び出し元に通知
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  // 空ファイルの時: buffer.fail() -> true, buffer.str() = "": 問題なし.
  // その他のエラーケースに関しても再現が困難なので現状エラー確認なし.
  file.close();
  return buffer.str();
}

bool remove_file(const std::string &file_path) {
  int ret = std::remove(file_path.c_str());
  if (ret == -1) {
    error_log_with_errno("remove_file");
    return false;
  }
  return true;
}

bool is_accessible(const std::string &file_path, int mode) {
  return access(file_path.c_str(), mode) == 0;
}
