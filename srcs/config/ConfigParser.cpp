#include "config/ConfigParser.hpp"

#include "config/Config.hpp"
#include "config/directive_parser.hpp"

namespace config {

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser() {}

ConfigParser::directiveParserMap ConfigParser::directive_parser_map_ =
    _init_directive_parser_map();

ConfigParser::directiveParserMap ConfigParser::_init_directive_parser_map() {
  directiveParserMap directive_parser_map;
  // clang-format off
  directive_parser_map["client_max_body_size"] = &ConfigParser::parse_client_max_body_size_directive;
  directive_parser_map["listen"] = &ConfigParser::parse_listen_directive;
  directive_parser_map["error_page"] = &ConfigParser::parse_error_page_directive;
  directive_parser_map["server_name"] = &ConfigParser::parse_server_name_directive;
  directive_parser_map["location"] = &ConfigParser::parse_location_directive;
  // clang-format on
  return directive_parser_map;
}

static void check_config_error(const Config &config) {
  if (config.locations_.empty()) {
    ERROR_EXIT("could not detect location directive.");
  }
  if (!config.has_listen_) {
    ERROR_EXIT("could not detect listen directive.");
  }
}

Config ConfigParser::_parse_config(tokenIterator pos, tokenIterator end) {
  pos++;
  if (*pos++ != "{")
    ERROR_EXIT("server directive does not have context.");
  while (pos != end && *pos != "}") {
    directiveParserMap::iterator it = directive_parser_map_.find(*pos);
    if (it == directive_parser_map_.end()) {
      ERROR_EXIT("unknown directive: " + *pos);
    }
    pos = (this->*it->second)(++pos, end);
  }
  if (pos == end) {
    ERROR_EXIT("could not detect context end.");
  }
  check_config_error(_config_);
  _last_iterator_pos_ = ++pos;
  Config res          = _config_;
  _config_            = Config();
  return res;
}

tokenIterator ConfigParser::parse_location_directive(tokenIterator pos,
                                                     tokenIterator end) {
  Location      location;
  tokenIterator result = location.parse_location(pos, end);
  _config_.locations_.add_or_exit(location);
  return result;
}

tokenIterator ConfigParser::parse_server_name_directive(tokenIterator pos,
                                                        tokenIterator end) {
  return parse_string_directive(_config_.server_name_, pos, end);
}

tokenIterator
ConfigParser::parse_client_max_body_size_directive(tokenIterator pos,
                                                   tokenIterator end) {
  tokenIterator token_iterator =
      parse_size_directive(_config_.client_max_body_size_, pos, end);
  if (_config_.client_max_body_size_ > MAX_CLIENT_MAX_BODY_SIZE_) {
    ERROR_EXIT("client_max_body_size is too large.");
  }
  return token_iterator;
}

tokenIterator ConfigParser::parse_error_page_directive(tokenIterator pos,
                                                       tokenIterator end) {
  return parse_map_directive(_config_.error_pages_, pos, end);
}

static struct sockaddr_in create_sockaddr_in(const std::string &listen_address,
                                             const std::string &listen_port) {
  struct addrinfo  hints    = {};
  struct addrinfo *addrinfo = NULL;
  hints.ai_family           = AF_INET;
  hints.ai_socktype         = SOCK_STREAM;
  int error = getaddrinfo(listen_address.c_str(), listen_port.c_str(), &hints,
                          &addrinfo);
  if (error != 0) {
    ERROR_EXIT("getaddrinfo: " << gai_strerror(error));
  }
  struct sockaddr_in *tmp = reinterpret_cast<sockaddr_in *>(addrinfo->ai_addr);
  struct sockaddr_in  sockaddr_in;
  sockaddr_in.sin_family = hints.ai_family;
  sockaddr_in.sin_addr   = tmp->sin_addr;
  sockaddr_in.sin_port   = tmp->sin_port;
  freeaddrinfo(addrinfo);
  return sockaddr_in;
}

tokenIterator ConfigParser::parse_listen_directive(tokenIterator pos,
                                                   tokenIterator end) {
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    ERROR_EXIT("could not detect directive value.");
  // LOG("listen: " << *pos);
  tokenVector   token_vector = tokenize(*pos, ":", "");
  tokenIterator it           = token_vector.begin();
  if (it == token_vector.end()) {
    ERROR_EXIT("could not detect listen address.");
  }
  std::string listen_address = *it++;
  // LOG("listen address: " << listen_address);
  if (it == token_vector.end()) {
    ERROR_EXIT("could not detect listen port.");
  }
  ++it;
  std::string listen_port = *it;
  _config_.sockaddr_in_   = create_sockaddr_in(listen_address, listen_port);
  _config_.has_listen_    = true;
  return pos + 2;
}

ConfigGroups ConfigParser::generate_config_group(const Path config_file_path) {
  ConfigGroups config_groups;

  Result<std::string> result = config_file_path.read_file_to_str();
  if (result.is_err_) {
    ERROR_EXIT(config_file_path << " is not found or can't read.");
  }
  tokenVector token_vector =
      tokenize(result.object_, CONFIG_DELIMITER, CONFIG_SKIP);
  tokenIterator it = token_vector.begin();
  while (it != token_vector.end()) {
    if (*it == "server") {
      Config config = _parse_config(it, token_vector.end());
      it            = _last_iterator_pos_;
      ConfigGroups::iterator joinable_config_group =
          find_joinable_config_group(config, config_groups);
      if (joinable_config_group != config_groups.end()) {
        joinable_config_group->add_config_or_exit(config);
      } else {
        config_groups.push_back(ConfigGroup(config));
      }
    } else {
      ERROR_EXIT("unexpected token: " << *it);
    }
  }

  return config_groups;
}

} // namespace config
