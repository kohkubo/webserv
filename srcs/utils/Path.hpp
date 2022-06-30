#ifndef SRCS_UTILS_PATH_HPP
#define SRCS_UTILS_PATH_HPP

#include <string>

#include "utils/Result.hpp"

class Path {
private:
  std::string _path_;

public:
  Path(const std::string &path);
  bool                is_file_exists();
  bool                is_dir_exists();
  bool                is_accessible(int mode);
  bool                is_minus_depth();
  Result<std::string> get_realpath();
  // tmp
  const std::string &path() const;
};

std::ostream &operator<<(std::ostream &os, const Path &path);

#endif /* SRCS_UTILS_PATH_HPP */
