#include "config/ConfigGroup.hpp"

ConfigGroup::ConfigGroup(const Config config)
    : _default_server_name_(config.server_name_) {
  _config_group_.insert(std::make_pair(config.server_name_, config));
}

struct sockaddr_in ConfigGroup::sockaddr_in() const {
  return _config_group_.begin()->second.sockaddr_in_;
}

bool ConfigGroup::is_same_socket(const Config &config) {
  struct sockaddr_in x = config.sockaddr_in_;
  struct sockaddr_in y = _config_group_.begin()->second.sockaddr_in_;
  return (x.sin_addr.s_addr == y.sin_addr.s_addr) && (x.sin_port == y.sin_port);
}

void ConfigGroup::add_config_or_exit(const Config config) {
  bool has_server_name =
      static_cast<bool>(_config_group_.count(config.server_name_));
  if (has_server_name) {
    ERROR_EXIT("server_name conflicts.");
  }
  _config_group_.insert(std::make_pair(config.server_name_, config));
}

Config ConfigGroup::select_config(const std::string &host_name) const {
  std::map<std::string, Config>::const_iterator proper_conf =
      _config_group_.find(host_name);
  if (proper_conf != _config_group_.end()) {
    return proper_conf->second;
  }
  return _config_group_.find(_default_server_name_)->second;
}
