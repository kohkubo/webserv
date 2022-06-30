#include "utils/file_io_utils.hpp"

#include <fcntl.h>
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
    ERROR_LOG("file not exists: " << path);
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

Result<std::string> read_file_to_str(const std::string &path) {
  std::ifstream file(path.c_str());
  if (file.fail()) {
    return Error<std::string>();
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  // 空ファイルの時: buffer.fail() -> true, buffer.str() = "": 問題なし.
  // その他のエラーケースに関しても再現が困難なので現状エラー確認なし.
  file.close();
  return Ok<std::string>(buffer.str());
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

Result<std::string> read_fd(int fd) {
  std::stringstream buffer;
  char              buf[1024];
  while (true) {
    ssize_t read_size = read(fd, buf, sizeof(buf));
    if (read_size == -1) {
      return Error<std::string>();
    }
    if (read_size == 0) {
      break;
    }
    buffer << std::string(buf, read_size);
  }
  return Ok<std::string>(buffer.str());
}

Result<int> open_read_file(const std::string &target_path) {
  int fd = open(target_path.c_str(), O_RDONLY);
  if (fd < 0) {
    ERROR_LOG("open error: " << target_path);
    return Error<int>();
  }
  return Ok<int>(fd);
}

Result<int> open_write_file(const std::string &target_path) {
  int fd = open(target_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
  if (fd < 0) {
    ERROR_LOG("open error: " << target_path);
    return Error<int>();
  }
  return Ok<int>(fd);
}
