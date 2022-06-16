#include "config/ConfGroupMapGenerator.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "config/Config.hpp"
#include "config/SocketOpener.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

ConfGroupMapGenerator::ConfGroupMapGenerator(const char *config_file_path) {
  _read_config(config_file_path);
}

void ConfGroupMapGenerator::_read_config(const char *config_file_path) {
  Result result = read_file_to_str(config_file_path);
  if (result.is_err_) {
    ERROR_EXIT(config_file_path << " is not found or can't read.");
  }
  tokenVector token_vector =
      tokenize(result.str_, CONFIG_DELIMITER, CONFIG_SKIP);
  tokenIterator it = token_vector.begin();
  while (it != token_vector.end()) {
    if (*it == "server") {
      Config config(it, token_vector.end());
      it = config.last_iterator_pos();
      _server_list_.push_back(config);
    } else {
      ERROR_EXIT("unexpected token: " << *it);
    }
  }
}

static bool is_same_socket(const Config &serv_x, const Config &serv_y) {
  struct sockaddr_in *x = NULL;
  struct sockaddr_in *y = NULL;
  x = reinterpret_cast<sockaddr_in *>(serv_x.addrinfo_->ai_addr);
  y = reinterpret_cast<sockaddr_in *>(serv_y.addrinfo_->ai_addr);
  return (x->sin_addr.s_addr == y->sin_addr.s_addr) &&
         (x->sin_port == y->sin_port);
}

static std::map<listenFd, confGroup>::iterator
find_same_socket(const Config                  &conf,
                 std::map<listenFd, confGroup> &confgroup_map) {
  std::map<listenFd, confGroup>::iterator it = confgroup_map.begin();
  for (; it != confgroup_map.end(); it++) {
    if (is_same_socket(conf, *(it->second[0])))
      break;
  }
  return it;
}

static bool is_include_same_server_name(const Config &conf,
                                        confGroup     conf_group) {
  confGroup::iterator it = conf_group.begin();
  for (; it != conf_group.end(); it++) {
    if ((*it)->server_name_ == conf.server_name_)
      return true;
  }
  return false;
}

std::map<listenFd, confGroup> ConfGroupMapGenerator::generate() {
  std::map<listenFd, confGroup> confgroup_map;
  serverList::const_iterator    sl_it = _server_list_.begin();
  for (; sl_it != _server_list_.end(); sl_it++) {
    std::map<listenFd, confGroup>::iterator it =
        find_same_socket(*sl_it, confgroup_map);
    if (it != confgroup_map.end()) {
      if (is_include_same_server_name(*sl_it, it->second)) {
        LOG("server_name conflicts.");
        continue;
      }
      it->second.push_back(&(*sl_it));
    } else {
      listenFd listen_fd = SocketOpener::open_new_socket(sl_it->addrinfo_);
      confgroup_map.insert(std::make_pair(listen_fd, confGroup()));
      confgroup_map[listen_fd].push_back(&(*sl_it));
    }
  }
  return confgroup_map;
}