#ifndef INCLUDES_RESOURCE_HPP
#define INCLUDES_RESOURCE_HPP

#include <string>

struct Resource {
  static const std::string hello_world_page_;
  static const std::string forbidden_page_;
  static const std::string not_found_page_;
  static const std::string bad_request_page_;
  static const std::string not_implemented_page_;
  static const std::string internal_server_error_page_;
  static const std::string unknown_error_page_;
  static const std::string root_;
  static const std::string text_html_;
};

#endif /* INCLUDES_RESOURCE_HPP */
