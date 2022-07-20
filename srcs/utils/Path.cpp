#include "utils/Path.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include "tokenize.hpp"
#include "utils/utils.hpp"

Path::Path(const std::string &path)
    : _path_(path) {}

Path::Path(const char *path)
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

bool Path::is_accessible(int mode) const {
  return access(_path_.c_str(), mode) == 0;
}

Result<std::string> Path::realpath() const {
  char *abs_path = ::realpath(_path_.c_str(), NULL);
  if (abs_path == NULL) {
    ERROR_LOG("realpath");
    return Error<std::string>();
  }
  std::string res = std::string(abs_path);
  free(abs_path);
  return Ok<std::string>(res);
}

std::string Path::script_name() const {
  std::size_t last_slash = _path_.find_last_of('/');
  return _path_.substr(last_slash + 1);
}

std::string Path::dirname() const {
  std::size_t last_slash = _path_.find_last_of('/');
  return _path_.substr(0, last_slash);
}

bool Path::is_minus_depth() const {
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

bool Path::remove_file() const {
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

Result<DIR *> Path::open_dir() const {
  DIR *dir = opendir(_path_.c_str());
  if (dir == NULL) {
    ERROR_LOG("opendir() failed");
    return Error<DIR *>();
  }
  return Ok<DIR *>(dir);
}

Result<std::string> Path::read_file_to_str() const {
  std::ifstream file(_path_.c_str());
  if (file.fail()) {
    return Error<std::string>();
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();
  return Ok<std::string>(buffer.str());
}

Path Path::operator+(const Path &rhs) const { return Path(str() + rhs.str()); }

const std::string &Path::str() const { return _path_; }

std::ostream &operator<<(std::ostream &os, const Path &path) {
  os << path.str();
  return os;
}

bool operator==(const Path &lhs, const Path &rhs) {
  return lhs.str() == rhs.str();
}
