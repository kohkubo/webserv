#ifndef SRCS_CONFIG_CONFIG_HPP
#define SRCS_CONFIG_CONFIG_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "config/Location.hpp"
#include "config/Locations.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

const std::string CONFIG_DELIMITER = "\v\r\f\t\n {};";
const std::string CONFIG_SKIP      = "\v\r\f\t\n ";

typedef std::map<int, std::string> errorPageMap;

class Config {
private:
  tokenIterator _last_iterator_pos_;

public:
  size_t             client_max_body_size_;
  std::string        server_name_;
  bool               has_listen_;
  struct sockaddr_in sockaddr_in_;
  errorPageMap       error_pages_;
  Locations          locations_;

public:
  class UnexpectedTokenException : public std::logic_error {
  public:
    UnexpectedTokenException(const std::string &msg = "unexpected token.");
  };

public:
  Config();
  Config(tokenIterator start, tokenIterator end)
      : client_max_body_size_(1024)
      , server_name_("")
      , has_listen_(false) {
    memset(&sockaddr_in_, 0, sizeof(sockaddr_in_));
    _last_iterator_pos_ = _parse(start, end);
  }
  ~Config(){};
  Config(const Config &other);
  Config       &operator=(const Config &other);
  tokenIterator last_iterator_pos() { return _last_iterator_pos_; }

private:
  tokenIterator        _parse(tokenIterator pos, tokenIterator end);
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
