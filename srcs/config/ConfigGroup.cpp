#include "config/ConfigGroup.hpp"

ConfigGroup::ConfigGroup(const Config config)
    : _default_server_name_(config.server_name_) {
  _config_group_.insert(std::make_pair(config.server_name_, config));
}

struct addrinfo *ConfigGroup::addrinfo() const {
  return _config_group_.begin()->second.addrinfo_;
}

bool ConfigGroup::is_same_socket(const Config &config) {
  struct sockaddr_in *x =
      reinterpret_cast<sockaddr_in *>(config.addrinfo_->ai_addr);
  struct sockaddr_in *y = reinterpret_cast<sockaddr_in *>(
      _config_group_.begin()->second.addrinfo_->ai_addr);
  return (x->sin_addr.s_addr == y->sin_addr.s_addr) &&
         (x->sin_port == y->sin_port);
}

bool ConfigGroup::try_add_config(const Config config) {
  bool has_server_name =
      static_cast<bool>(_config_group_.count(config.server_name_));
  if (has_server_name) {
    LOG("server_name conflicts.");
    return false;
  }
  _config_group_.insert(std::make_pair(config.server_name_, config));
  return true;
}

Config ConfigGroup::select_config(const std::string &host_name) const {
  std::map<std::string, Config>::const_iterator proper_conf =
      _config_group_.find(host_name);
  if (proper_conf != _config_group_.end()) {
    return proper_conf->second;
  }
  return _config_group_.find(_default_server_name_)->second;
}
