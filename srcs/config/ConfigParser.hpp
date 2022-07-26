#ifndef SRCS_CONFIG_CONFIGPARSER
#define SRCS_CONFIG_CONFIGPARSER

#include <vector>

#include "config/Config.hpp"
#include "config/ConfigGroup.hpp"
#include "utils/Path.hpp"
#include "utils/Result.hpp"
#include "utils/tokenize.hpp"

namespace config {

class ConfigParser {
public:
  typedef tokenIterator (ConfigParser::*directive_parser_func)(
      tokenIterator pos, tokenIterator end);
  typedef std::map<std::string, directive_parser_func> directiveParserMap;

private:
  Config                    _config_;
  tokenIterator             _last_iterator_pos_;
  static directiveParserMap directive_parser_map_;
  static const size_t MAX_CLIENT_MAX_BODY_SIZE_ = 65536; //数字には根拠なし

private:
  static directiveParserMap _init_directive_parser_map();
  Config                    _parse_config(tokenIterator pos, tokenIterator end);

public:
  ConfigParser();
  ~ConfigParser();
  std::vector<ConfigGroup> generate_config_group(const Path config_file_path);
  tokenIterator            last_iterator_pos() { return _last_iterator_pos_; }

  // clang-format off
  tokenIterator parse_client_max_body_size_directive(tokenIterator pos, tokenIterator end);
  tokenIterator parse_server_name_directive(tokenIterator pos, tokenIterator end);
  tokenIterator parse_listen_directive(tokenIterator pos, tokenIterator end);
  tokenIterator parse_error_page_directive(tokenIterator pos, tokenIterator end);
  tokenIterator parse_location_directive(tokenIterator pos, tokenIterator end);
  // clang-format on
};

} // namespace config

#endif /* SRCS_CONFIG_CONFIGPARSER */
