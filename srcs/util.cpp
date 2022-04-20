#include "util.hpp"
#include "Tokens.hpp"
#include <fstream>
#include <limits>
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
    std::cout << "is_file_exists" << std::endl;
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
    std::cout << "read_file_tostring" << std::endl;
    return ""; // TODO: エラーを呼び出し元に通知
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  // 空ファイルの時: buffer.fail() -> true, buffer.str() = "": 問題なし.
  // その他のエラーケースに関しても再現が困難なので現状エラー確認なし.
  file.close();
  return buffer.str();
}

bool is_uint8(const std::string &token) {
  if (token.size() == 1) {
    if (token[0] >= '0' && token[0] <= '9') {
      return true;
    }
    return false;
  } else if (token.size() == 2) {
    if (token[0] >= '1' && token[0] <= '9' && token[1] >= '0' &&
        token[1] <= '9') {
      return true;
    }
    return false;
  } else if (token.size() == 3) {
    if (token[0] == '1') {
      if ((token[1] >= '0' && token[1] <= '9') &&
          (token[2] >= '0' && token[2] <= '9'))
        return true;
    }
    if (token[0] == '2') {
      if ((token[1] >= '0' && token[1] <= '4') &&
          (token[2] >= '0' && token[2] <= '9'))
        return true;
      if (token[1] == '5' && (token[2] >= '0' && token[2] <= '5'))
        return true;
    }
  }
  return false;
}

bool is_ip(const std::string &token) {
  Tokens                 l(token, ".", "");
  Tokens::token_iterator it    = l.begin();
  size_t                 count = 0;
  for (; it != l.end(); it++, count++) {
    if (!is_uint8(*it))
      return false;
    it++;
    if (count == 3)
      break;
    if (it != l.end() && *it != ".")
      return false;
  }
  return true;
}

bool is_digits(const std::string &token) {
  for (size_t i = 0; i < token.size(); i++) {
    if (!isdigit(token[i]))
      return false;
  }
  return true;
}
