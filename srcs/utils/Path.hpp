#ifndef SRCS_UTILS_PATH_HPP
#define SRCS_UTILS_PATH_HPP

#include <string>

#include "utils/Result.hpp"

class Path {
private:
  std::string _path_;

private:
  static std::string _normalize_path(const std::string &path);

public:
  Path(const std::string &path);
  bool                is_file_exists();
  bool                is_dir_exists();
  bool                is_accessible(int mode);
  Result<std::string> get_realpath();
};

#endif /* SRCS_UTILS_PATH_HPP */
