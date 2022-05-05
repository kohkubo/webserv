#include "utils/http_parser_utils.hpp"

// TODO: location
std::string resolve_url(const ServerConfig &server_config,
                        const std::string   request_url) {
  if (request_url == "/") {
    return server_config.root_ + "/" + server_config.index_;
  } else {
    return server_config.root_ + "/" + request_url;
  }
}
