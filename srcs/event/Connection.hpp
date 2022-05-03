#ifndef SRCS_EVENT_CONNECTION_HPP
#define SRCS_EVENT_CONNECTION_HPP

#include <string>
#include <map>

struct Connection {
  int         listen_fd_;
  std::string data_;

  Connection(int socket_fd) : listen_fd_(socket_fd) {}
};

typedef std::map<int, Connection> connection_list_type;

#endif /* SRCS_EVENT_CONNECTION_HPP */