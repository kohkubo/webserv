#ifndef SRCS_CONFIG_LOCATION_HPP
#define SRCS_CONFIG_LOCATION_HPP

#include <map>
#include <string>
#include <vector>

struct Location {
  std::string                location_path_;
  std::string                root_;
  std::string                index_;
  bool                       autoindex_;
  std::map<int, std::string> return_;
  std::vector<std::string>   limit_except_;
  bool                       cgi_extension_;

  Location()
      : root_("/")
      , index_("index.html")
      , autoindex_(false)
      , return_()
      , limit_except_() {}
};

#endif /* SRCS_CONFIG_LOCATION_HPP */
