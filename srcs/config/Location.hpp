#ifndef SRCS_CONFIG_LOCATION_HPP
#define SRCS_CONFIG_LOCATION_HPP

#include <map>
#include <string>
#include <vector>

#include "utils/Path.hpp"

typedef std::map<int, std::string> returnMap;

struct Location {
  Path                     location_path_;
  Path                     root_;
  Path                     index_;
  bool                     autoindex_;
  bool                     cgi_extension_;
  bool                     upload_file_;
  returnMap                return_map_;
  std::vector<std::string> available_methods_;

  Location()
      : root_("html/")
      , index_("index.html")
      , autoindex_(false)
      , cgi_extension_(false)
      , upload_file_(false) {
    available_methods_.push_back("GET");
    available_methods_.push_back("POST");
    available_methods_.push_back("DELETE");
  }
};

#endif /* SRCS_CONFIG_LOCATION_HPP */
