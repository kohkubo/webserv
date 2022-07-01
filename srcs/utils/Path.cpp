#include "utils/Path.hpp"

#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>

#include "tokenize.hpp"
#include "utils/utils.hpp"

Path::Path(const std::string &path)
    : _path_(path) {}

bool Path::is_file_exists() const {
  struct stat file_info = {};

  if (stat(_path_.c_str(), &file_info) == -1) {
    ERROR_LOG("file not exists: " << _path_);
    return false;
  }
  return ((file_info.st_mode & S_IFMT) == S_IFREG);
}

bool Path::is_dir_exists() const {
  struct stat file_info = {};

  if (stat(_path_.c_str(), &file_info) == -1) {
    ERROR_LOG("error: is_dir_exists: " << _path_);
    return false;
  }
  return ((file_info.st_mode & S_IFMT) == S_IFDIR);
}

bool Path::has_suffix(const std::string &suffix) const {
  return ::has_suffix(_path_, suffix);
}

bool Path::is_accessible(int mode) { return access(_path_.c_str(), mode) == 0; }

Result<std::string> Path::get_realpath() const {
  char *abs_path = realpath(_path_.c_str(), NULL);
  if (abs_path == NULL) {
    ERROR_LOG("get_realpath");
    return Error<std::string>();
  }
  std::string res = std::string(abs_path);
  free(abs_path);
  return Ok<std::string>(res);
}

bool Path::is_minus_depth() { return ::is_minus_depth(_path_); }

bool Path::remove_file() {
  int ret = std::remove(_path_.c_str());
  if (ret == -1) {
    ERROR_LOG_WITH_ERRNO("remove_file");
    return false;
  }
  return true;
}

Result<int> Path::open_read_file() const {
  int fd = open(_path_.c_str(), O_RDONLY);
  if (fd < 0) {
    ERROR_LOG("open error: " << _path_);
    return Error<int>();
  }
  return Ok<int>(fd);
}

Result<int> Path::open_write_file() const {
  int fd = open(_path_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
  if (fd < 0) {
    ERROR_LOG("open error: " << _path_);
    return Error<int>();
  }
  return Ok<int>(fd);
}

Path Path::operator+(const Path &rhs) const { return Path(str() + rhs.str()); }

const std::string &Path::str() const { return _path_; }

std::ostream &operator<<(std::ostream &os, const Path &path) {
  os << path.str();
  return os;
}
