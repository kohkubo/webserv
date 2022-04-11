#ifndef INCLUDES_CONFIG_LOCATION_HPP
#define INCLUDES_CONFIG_LOCATION_HPP

#include <string>
#include <vector>

class Location {
public:
  std::string              root_;
  std::vector<std::string> index_;
  bool                     autoindex_;

  std::string              upload_store_;
  // CGI
  // redirect
public:
  Location();
  ~Location();
};

Location::Location() {}

Location::~Location() {}

#endif /* INCLUDES_CONFIG_LOCATION_HPP */
