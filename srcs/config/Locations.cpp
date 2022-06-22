#include "config/Locations.hpp"

#include <algorithm>
#include <string>

bool Locations::add_or_else(const Location &location) {
  std::string                     location_path = location.location_path_;
  std::vector<Location>::iterator it            = _location_vector_.begin();
  for (; it != _location_vector_.end(); ++it) {
    if (it->location_path_ == location_path) {
      return false;
    }
  }
  _location_vector_.push_back(location);
  return true;
}

const Location *
Locations::select_location(const std::string &request_target) const {
  const Location *location = NULL;
  std::string     path;

  std::vector<Location>::const_iterator it = _location_vector_.begin();
  for (; it != _location_vector_.end(); ++it) {
    if (request_target.find(it->location_path_) == 0) {
      if (path.size() < it->location_path_.size()) {
        path     = it->location_path_;
        location = &(*it);
      }
    }
  }
  return location;
}