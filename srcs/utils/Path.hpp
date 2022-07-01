#ifndef SRCS_UTILS_PATH_HPP
#define SRCS_UTILS_PATH_HPP

#include <dirent.h>
#include <sys/types.h>

#include <string>

#include "utils/Result.hpp"

class Path {
private:
  std::string _path_;

public:
  Path(const std::string &path);
  Path(char const *path);
  bool                is_file_exists() const;
  bool                is_dir_exists() const;
  bool                is_accessible(int mode);
  bool                is_minus_depth();
  bool                has_suffix(const std::string &suffix) const;
  Result<std::string> get_realpath() const;
  bool                remove_file();
  Result<int>         open_read_file() const;
  Result<int>         open_write_file() const;
  Result<DIR *>       open_dir() const;
  Result<std::string> read_file_to_str() const;
  Path                operator+(const Path &rhs) const;
  // tmp
  const std::string &str() const;
};

std::ostream &operator<<(std::ostream &os, const Path &path);

#endif /* SRCS_UTILS_PATH_HPP */