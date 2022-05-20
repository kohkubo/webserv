#ifndef SRCS_CONFIG_SERVERCONFIG_HPP
#define SRCS_CONFIG_SERVERCONFIG_HPP

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "utils/tokenize.hpp"

const std::string CONFIG_DELIMITER = "\v\r\f\t\n {};";
const std::string CONFIG_SKIP      = "\v\r\f\t\n ";

class Config {
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
  Config();
  ~Config();
  Config(const Config &other);
  Config       &operator=(const Config &other);
  tokenIterator parse(tokenIterator pos, tokenIterator end);

private:
  void          __set_getaddrinfo();
  tokenIterator __parse_listen(tokenIterator pos, tokenIterator end);
  tokenIterator __parse_map_directive(std::string                 key,
                                      std::map<int, std::string> &value,
                                      tokenIterator pos, tokenIterator end);
  tokenIterator __parse_string_directive(std::string key, std::string &value,
                                         tokenIterator pos, tokenIterator end);
  tokenIterator __parse_sizet_directive(std::string key, size_t &value,
                                        tokenIterator pos, tokenIterator end);
  tokenIterator __parse_bool_directive(std::string key, bool &value,
                                       tokenIterator pos, tokenIterator end);
  tokenIterator __parse_vector_directive(std::string               key,
                                         std::vector<std::string> &value,
                                         tokenIterator pos, tokenIterator end);
};

// TODO: Config ポインタ -> 実体
// conf_group: 同じソケットのserver_confの集合
typedef std::vector<Config>         serverList;
typedef std::vector<const Config *> confGroup;
typedef int                         listenFd;

#endif /* SRCS_CONFIG_SERVERCONFIG_HPP */
