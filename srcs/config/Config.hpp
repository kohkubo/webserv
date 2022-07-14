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

namespace config {

class Config;
typedef std::map<int, std::string> errorPageMap;

class Config {
  typedef tokenIterator (Config::*directive_parser_func)(tokenIterator pos,
                                                         tokenIterator end);
  typedef std::map<std::string, directive_parser_func> directiveParserMap;

private:
  tokenIterator _last_iterator_pos_;

public:
  size_t             client_max_body_size_;
  std::string        server_name_;
  bool               has_listen_;
  struct sockaddr_in sockaddr_in_;
  errorPageMap       error_pages_;
  Locations          locations_;
  directiveParserMap directive_parser_map_;
  static const size_t MAX_CLIENT_MAX_BODY_SIZE_ = 65536; //数字には根拠なし

public:
  class UnexpectedTokenException : public std::logic_error {
  public:
    UnexpectedTokenException(const std::string &msg = "unexpected token.");
  };

public:
  Config();
  Config(tokenIterator start, tokenIterator end)
      : client_max_body_size_(MAX_CLIENT_MAX_BODY_SIZE_)
      , server_name_("")
      , has_listen_(false) {
    _init_directive_parser_map();
    memset(&sockaddr_in_, 0, sizeof(sockaddr_in_));
    _last_iterator_pos_ = _parse(start, end);
  }
  ~Config(){};
  Config(const Config &other);
  Config       &operator=(const Config &other);
  tokenIterator last_iterator_pos() { return _last_iterator_pos_; }

private:
  tokenIterator _parse(tokenIterator pos, tokenIterator end);

  // clang-format off
  tokenIterator _parse_client_max_body_size_directive(tokenIterator pos, tokenIterator end);
  tokenIterator _parse_server_name_directive(tokenIterator pos, tokenIterator end);
  tokenIterator _parse_listen_directive(tokenIterator pos, tokenIterator end);
  tokenIterator _parse_error_page_directive(tokenIterator pos, tokenIterator end);
  tokenIterator _parse_location_directive(tokenIterator pos, tokenIterator end);
  // clang-format on
  void _init_directive_parser_map();
};

} // namespace config

#endif /* SRCS_CONFIG_CONFIG_HPP */
