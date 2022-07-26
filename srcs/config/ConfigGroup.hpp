#ifndef SRCS_CONFIG_CONFIGGROUP_HPP
#define SRCS_CONFIG_CONFIGGROUP_HPP

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <map>
#include <string>
#include <vector>

#include "config/Config.hpp"

namespace config {

class ConfigGroup;
typedef std::vector<ConfigGroup> ConfigGroups;

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

ConfigGroups::iterator find_joinable_config_group(const Config &config,
                                                  ConfigGroups &config_groups);

} // namespace config

#endif /* SRCS_CONFIG_CONFIGGROUP_HPP */
