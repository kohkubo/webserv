#include "utils/Path.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include "tokenize.hpp"
#include "utils/utils.hpp"

Path::Path(const std::string &path)
    : _path_(path) {}

bool Path::is_file_exists() {
  struct stat file_info = {};

  if (stat(_path_.c_str(), &file_info) == -1) {
    ERROR_LOG("file not exists: " << _path_);
    return false;
  }
  return ((file_info.st_mode & S_IFMT) == S_IFREG);
}

bool Path::is_dir_exists() {
  struct stat file_info = {};

  if (stat(_path_.c_str(), &file_info) == -1) {
    ERROR_LOG("error: is_dir_exists: " << _path_);
    return false;
  }
  return ((file_info.st_mode & S_IFMT) == S_IFDIR);
}

bool Path::is_accessible(int mode) { return access(_path_.c_str(), mode) == 0; }

Result<std::string> Path::get_realpath() {
  char *abs_path = realpath(_path_.c_str(), NULL);
  if (abs_path == NULL) {
    ERROR_LOG("get_realpath");
    return Error<std::string>();
  }
  std::string res = std::string(abs_path);
  free(abs_path);
  return Ok<std::string>(res);
}

bool Path::is_minus_depth() {
  tokenVector   tokens = tokenize(_path_, "/", "/");
  tokenIterator it     = tokens.begin();

  for (long depth = 0; it != tokens.end(); it++) {
    if (*it == "..") {
      depth--;
    } else if (*it != ".") {
      depth++;
    }
    if (depth < 0) {
      return true;
    }
  }
  return false;
}

Path Path::operator+(const Path &rhs) const {
  return Path(path() + rhs.path());
}

const std::string &Path::path() const { return _path_; }

std::ostream &operator<<(std::ostream &os, const Path &path) {
  os << path.path();
  return os;
}
