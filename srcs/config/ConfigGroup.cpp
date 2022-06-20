#include "config/ConfigGroup.hpp"

ConfigGroup::ConfigGroup(const Config config)
    : _default_server_name_(config.server_name_) {
  _config_list_.push_back(config);
  const Config *ptr = &_config_list_.back();
  _config_group_.insert(std::make_pair(config.server_name_, ptr));
}

bool ConfigGroup::is_same_socket(const Config &config) {
  struct sockaddr_in *x = NULL;
  struct sockaddr_in *y = NULL;
  x = reinterpret_cast<sockaddr_in *>(config.addrinfo_->ai_addr);
  y = reinterpret_cast<sockaddr_in *>(_config_list_[0].addrinfo_->ai_addr);
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
  _config_list_.push_back(config);
  const Config *ref = &_config_list_.back();
  _config_group_.insert(std::make_pair(config.server_name_, ref));
  return true;
}
