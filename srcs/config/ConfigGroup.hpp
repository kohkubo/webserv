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

namespace config {

class ConfigGroup {
private:
  std::string                         _default_server_name_;
  std::map<std::string, const Config> _config_group_;

public:
  ConfigGroup(const Config &config);
  ~ConfigGroup() {}
  struct sockaddr_in sockaddr_in() const;
  bool               is_same_socket(const Config &config);
  void               add_config_or_exit(const Config &config);
  const Config      *select_config(const std::string &host_name) const;
};

} // namespace config

#endif /* SRCS_CONFIG_CONFIGGROUP_HPP */
