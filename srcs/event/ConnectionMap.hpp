#ifndef SRCS_EVENT_CONNECTIONMAP_HPP
#define SRCS_EVENT_CONNECTIONMAP_HPP

#include <map>

#include "config/ServerConfig.hpp"
#include "event/Connection.hpp"

class ConnectionMap {
private:
  std::map<int, Connection> __container_;

public:
  typedef std::map<int, Connection>::iterator       iterator;
  typedef std::map<int, Connection>::const_iterator const_iterator;

public:
  ConnectionMap() {}
  ~ConnectionMap() {}

  iterator       begin() { return __container_.begin(); }
  const_iterator begin() const { return __container_.begin(); }
  iterator       end() { return __container_.end(); }
  const_iterator end() const { return __container_.end(); }

  void add_new_connection(int listen_fd, socket_list_type &socket_list);
  void receive(int connection_fd);
  void send(int connection_fd);
};

#endif /* SRCS_EVENT_CONNECTIONMAP_HPP */
