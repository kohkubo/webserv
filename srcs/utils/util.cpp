#include "util.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

bool is_match_suffix_string(const std::string &str, const std::string &suffix) {
  if (str.length() < suffix.length()) {
    return false;
  }
  if (str.compare(str.length() - suffix.length(), suffix.length(), suffix) ==
      0) {
    return true;
  }
  return false;
}

bool is_file_exists(const char *path) {
  struct stat file_info;

  if (stat(path, &file_info) == -1) {
    std::cout << "error: is_file_exists" << std::endl;
    return false; // TODO: エラーを呼び出し元に通知
  }
  if ((file_info.st_mode & S_IFMT) == S_IFREG)
    return true;
  else
    return false;
}

std::string read_file_tostring(const char *path) {
  std::ifstream file(path);
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
