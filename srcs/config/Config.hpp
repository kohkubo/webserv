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
#include "utils/utils.hpp"

const std::string CONFIG_DELIMITER = "\v\r\f\t\n {};";
const std::string CONFIG_SKIP      = "\v\r\f\t\n ";

typedef std::map<int, std::string> errorPageMap;

class Config {
private:
  tokenIterator _last_iterator_pos_;

public:
  std::string           listen_address_;
  std::string           listen_port_;
  size_t                client_max_body_size_;
  std::string           server_name_;
  struct addrinfo      *addrinfo_;
  errorPageMap          error_pages_;
  std::vector<Location> locations_;

  // error_page;
public:
  class UnexpectedTokenException : public std::logic_error {
  public:
    UnexpectedTokenException(const std::string &msg = "unexpected token.");
  };

public:
  Config();
  Config(tokenIterator start, tokenIterator end)
      : listen_address_("0.0.0.0")
      , listen_port_("80")
      , client_max_body_size_(1024)
      , server_name_("")
      , addrinfo_(NULL) {
    try {
      _last_iterator_pos_ = _parse(start, end);
    } catch (const std::exception &e) {
      ERROR_EXIT(e.what());
    }
  }
  ~Config();
  Config(const Config &other);
  Config       &operator=(const Config &other);
  tokenIterator last_iterator_pos() { return _last_iterator_pos_; }

private:
  tokenIterator _parse(tokenIterator pos, tokenIterator end);
  static void _set_getaddrinfo(const std::string &host, const std::string &port,
                               struct addrinfo **addrinfo);
  tokenIterator        _parse_listen(tokenIterator pos, tokenIterator end);
  tokenIterator        _parse_location(tokenIterator pos, tokenIterator end);
  static tokenIterator _parse_map_directive(std::string                 key,
                                            std::map<int, std::string> &value,
                                            tokenIterator               pos,
                                            tokenIterator               end);
  static tokenIterator _parse_string_directive(std::string   key,
                                               std::string  &value,
                                               tokenIterator pos,
                                               tokenIterator end);
  static tokenIterator _parse_size_directive(std::string key, size_t &value,
                                             tokenIterator pos,
                                             tokenIterator end);
  static tokenIterator _parse_bool_directive(std::string key, bool &value,
                                             tokenIterator pos,
                                             tokenIterator end);
  static tokenIterator _parse_vector_directive(std::string               key,
                                               std::vector<std::string> &value,
                                               tokenIterator             pos,
                                               tokenIterator             end);
};

typedef int listenFd;

#endif /* SRCS_CONFIG_CONFIG_HPP */
