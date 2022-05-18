#ifndef SRCS_CONFIG_SERVERCONFIG_HPP
#define SRCS_CONFIG_SERVERCONFIG_HPP

#include "utils/tokenize.hpp"
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#define CONFIG_DELIMITER "\v\r\f\t\n {};"
#define CONFIG_SKIP      "\v\r\f\t\n "

class ServerConfig {
public:
  std::string                listen_address_;
  std::string                listen_port_;
  size_t                     client_max_body_size_;
  std::string                server_name_;
  std::string                root_;
  std::string                index_;
  std::map<int, std::string> error_pages_;
  std::map<int, std::string> return_;
  bool                       autoindex_;
  std::vector<std::string>   limit_except_;
  struct addrinfo           *addrinfo_;

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
  token_iterator __parse_map_directive(std::string                 key,
                                       std::map<int, std::string> &value,
                                       token_iterator pos, token_iterator end);
  token_iterator __parse_string_directive(std::string key, std::string &value,
                                          token_iterator pos,
                                          token_iterator end);
  token_iterator __parse_sizet_directive(std::string key, size_t &value,
                                         token_iterator pos,
                                         token_iterator end);
  token_iterator __parse_bool_directive(std::string key, bool &value,
                                        token_iterator pos, token_iterator end);
  token_iterator __parse_vector_directive(std::string               key,
                                          std::vector<std::string> &value,
                                          token_iterator            pos,
                                          token_iterator            end);
};

// TODO: ServerConfig ポインタ -> 実体
// conf_group: 同じソケットのserver_confの集合
typedef std::vector<ServerConfig>         server_list;
typedef std::vector<const ServerConfig *> conf_group;
typedef int                               listen_fd;

#endif /* SRCS_CONFIG_SERVERCONFIG_HPP */
