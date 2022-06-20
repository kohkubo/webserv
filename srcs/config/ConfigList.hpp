#ifndef SRCS_CONFIG_CONFIGLIST_HPP
#define SRCS_CONFIG_CONFIGLIST_HPP

#include <vector>

#include "config/Config.hpp"

class ConfigList {
private:
  std::vector<Config> _config_list_;

public:
  typedef std::vector<Config>::const_iterator const_iterator;

public:
  ConfigList();
  ConfigList(const char *config_file_path);
  ~ConfigList() {}
  const_iterator begin() const { return _config_list_.begin(); }
  const_iterator end() const { return _config_list_.end(); }
};

#endif /* SRCS_CONFIG_CONFIGLIST_HPP */
