#ifndef SRCS_CONFIG_LOCATION_HPP
#define SRCS_CONFIG_LOCATION_HPP

#include <map>
#include <string>
#include <vector>

typedef std::map<int, std::string> returnMap;

struct Location {
  std::string              location_path_;
  std::string              root_;
  std::string              index_;
  bool                     autoindex_;
  bool                     cgi_extension_;
  bool                     upload_file_;
  returnMap                return_map_;
  std::vector<std::string> limit_except_;

  Location()
      : root_("html/")
      , index_("index.html")
      , autoindex_(false)
      , cgi_extension_(false)
      , upload_file_(false) {
    limit_except_.push_back("GET");
    limit_except_.push_back("POST");
    limit_except_.push_back("DELETE");
  }
};

#endif /* SRCS_CONFIG_LOCATION_HPP */
