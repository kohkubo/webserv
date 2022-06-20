#ifndef SRCS_CONFIG_CONFIGGROUP_HPP
#define SRCS_CONFIG_CONFIGGROUP_HPP

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <map>
#include <string>
#include <vector>

#include "config/Config.hpp"

/*
  同じソケットを使うconfigのリスト
  Config自体はvectorで保存、server_nameをkeyにしたmapを持つ。
 */

class ConfigGroup {
private:
  std::vector<Config>                   _config_list_;
  std::map<std::string, const Config *> _config_group_;
  std::string                           _default_server_name_;

public:
  ConfigGroup() {}
  ConfigGroup(const Config config);
  ~ConfigGroup() {}
  bool is_same_socket(const Config &config);
  bool try_add_config(const Config config);
};

#endif /* SRCS_CONFIG_CONFIGGROUP_HPP */
