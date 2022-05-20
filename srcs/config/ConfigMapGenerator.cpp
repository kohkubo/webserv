#include "config/ConfigMapGenerator.hpp"

#include <string>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>

#include "config/Config.hpp"
#include "config/create_listen_fd_map.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/tokenize.hpp"

ConfigMapGenerator::ConfigMapGenerator(const char *config_file_path) {
  __read_config(config_file_path);
}

void ConfigMapGenerator::__read_config(const char *config_file_path) {
  std::string contents     = read_file_tostring(config_file_path);
  tokenVector token_vector = tokenize(contents, CONFIG_DELIMITER, CONFIG_SKIP);
  tokenIterator it         = token_vector.begin();
  while (it != token_vector.end()) {
    if (*it == "server") {
      Config config(it, token_vector.end());
      it = config.get_moved_it();
      __server_list_.push_back(config);
    } else {
      std::cerr << "unexpected token: " << *it << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

static bool is_same_socket(const Config &serv_x, const Config &serv_y) {
  struct sockaddr_in *x, *y;
  x = reinterpret_cast<sockaddr_in *>(serv_x.addrinfo_->ai_addr);
  y = reinterpret_cast<sockaddr_in *>(serv_y.addrinfo_->ai_addr);
  return (x->sin_addr.s_addr == y->sin_addr.s_addr) &&
         (x->sin_port == y->sin_port);
}

static std::map<listenFd, confGroup>::iterator
find_same_socket(const Config                  &conf,
                 std::map<listenFd, confGroup> &listen_fd_map) {
  std::map<listenFd, confGroup>::iterator it = listen_fd_map.begin();
  for (; it != listen_fd_map.end(); it++) {
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

std::map<listenFd, confGroup> ConfigMapGenerator::generate() {
  std::map<listenFd, confGroup> listen_fd_map;
  serverList::const_iterator    sl_it = __server_list_.begin();
  for (; sl_it != __server_list_.end(); sl_it++) {
    std::map<listenFd, confGroup>::iterator it =
        find_same_socket(*sl_it, listen_fd_map);
    if (it != listen_fd_map.end()) {
      if (is_include_same_server_name(*sl_it, it->second)) {
        std::cout << "server_name conflicts." << std::endl;
        continue;
      }
      it->second.push_back(&(*sl_it));
    } else {
      int new_socket = open_new_socket(*sl_it);
      listen_fd_map.insert(std::make_pair(new_socket, confGroup()));
      listen_fd_map[new_socket].push_back(&(*sl_it));
    }
  }
  return listen_fd_map;
}