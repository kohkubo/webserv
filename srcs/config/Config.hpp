#ifndef SRCS_CONFIG_CONFIG_HPP
#define SRCS_CONFIG_CONFIG_HPP

#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "config/Location.hpp"
#include "utils/tokenize.hpp"

const std::string CONFIG_DELIMITER = "\v\r\f\t\n {};";
const std::string CONFIG_SKIP      = "\v\r\f\t\n ";

class Config {
private:
  tokenIterator __last_it_;

public:
  std::string                listen_address_;
  std::string                listen_port_;
  size_t                     client_max_body_size_;
  std::string                server_name_;
  std::map<int, std::string> error_pages_;
  struct addrinfo           *addrinfo_;
  std::vector<Location>      locations_;

  // error_page;
public:
  class UnexpectedTokenException : public std::logic_error {
  public:
    UnexpectedTokenException(const std::string &msg = "unexpected token.");
  };

public:
  Config();
  // TODO: この初期化はテスト用に残っているだけ
  Config(tokenIterator start, tokenIterator end)
      : listen_address_("0.0.0.0")
      , listen_port_("80")
      , client_max_body_size_(0)
      , server_name_("")
      , error_pages_()
      , addrinfo_(NULL) {
    try {
      __last_it_ = __parse(start, end);
    } catch (const std::exception &e) {
      std::cerr << e.what() << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  ~Config();
  Config(const Config &other);
  Config       &operator=(const Config &other);
  tokenIterator get_moved_it() { return __last_it_; }

private:
  tokenIterator __parse(tokenIterator pos, tokenIterator end);
  void          __set_getaddrinfo();
  tokenIterator __parse_listen(tokenIterator pos, tokenIterator end);
  tokenIterator __parse_location(tokenIterator pos, tokenIterator end);
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

#endif /* SRCS_CONFIG_CONFIG_HPP */
