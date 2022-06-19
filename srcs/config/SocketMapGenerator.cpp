#include "config/SocketMapGenerator.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "config/Config.hpp"
#include "config/SocketOpener.hpp"
#include "socket/ListenSocket.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

SocketMapGenerator::SocketMapGenerator(const char *config_file_path) {
  _read_config(config_file_path);
}

void SocketMapGenerator::_read_config(const char *config_file_path) {
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

static std::map<int, SocketBase *>::iterator
find_same_socket(const Config                &conf,
                 std::map<int, SocketBase *> &confgroup_map) {
  std::map<int, SocketBase *>::iterator it = confgroup_map.begin();
  for (; it != confgroup_map.end(); it++) {
    if (is_same_socket(
            conf, *(dynamic_cast<ListenSocket *>(it->second))->conf_group_[0]))
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

std::map<int, SocketBase *> SocketMapGenerator::generate() {
  std::map<int, SocketBase *> socket_map;
  serverList::const_iterator  sl_it = _server_list_.begin();
  for (; sl_it != _server_list_.end(); sl_it++) {
    std::map<int, SocketBase *>::iterator it =
        find_same_socket(*sl_it, socket_map);
    if (it != socket_map.end()) {
      if (is_include_same_server_name(
              *sl_it,
              (dynamic_cast<ListenSocket *>(it->second))->conf_group_)) {
        LOG("server_name conflicts.");
        continue;
      }
      (dynamic_cast<ListenSocket *>(it->second))
          ->conf_group_.push_back(&(*sl_it));
    } else {
      listenFd    listen_fd = SocketOpener::open_new_socket(sl_it->addrinfo_);
      SocketBase *new_listen_socket = new ListenSocket(listen_fd, &(*sl_it));
      socket_map.insert(std::make_pair(listen_fd, new_listen_socket));
    }
  }
  return socket_map;
}