#ifndef SRCS_SERVERCONFIG_HPP
#define SRCS_SERVERCONFIG_HPP

#include "utils/tokenize.hpp"
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

class ServerConfig {
public:
  std::string      listen_address_;
  std::string      listen_port_;
  int              client_max_body_size_;
  std::string      server_name_;
  std::string      root_;
  std::string      index_;
  struct addrinfo *addrinfo_;

  // error_page;
public:
  class UnexpectedTokenException : public std::logic_error {
  public:
    UnexpectedTokenException(const std::string &msg = "unexpected token.");
  };

public:
  ServerConfig();
  ~ServerConfig();
  ServerConfig(const ServerConfig &other);
  ServerConfig  &operator=(const ServerConfig &other);
  token_iterator parse(token_iterator pos, token_iterator end);

private:
  void           __set_getaddrinfo();
  token_iterator __parse_listen(token_iterator pos, token_iterator end);
  token_iterator __parse_root(token_iterator pos, token_iterator end);
  token_iterator __parse_server_name(token_iterator pos, token_iterator end);
};

typedef std::vector<ServerConfig>                         server_list_type;
typedef std::vector<std::vector<const ServerConfig *> >   server_group_type;
typedef std::map<int, std::vector<const ServerConfig *> > socket_list_type;

server_group_type create_server_group(const server_list_type &server_list);

#endif /* SRCS_SERVERCONFIG_HPP */
