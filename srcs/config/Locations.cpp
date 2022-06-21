#include "config/Locations.hpp"

#include <string>

bool Locations::add_or_else(const Location *location) {
  std::vector<const Location *>::const_iterator it =
      std::find(_location_vector_.begin(), _location_vector_.end(), location);
  if (it != _location_vector_.end()) {
    return false;
  }
  _location_vector_.insert(_location_vector_.begin(), location);
  return true;
}

const Location *
Locations::select_location(const std::string &request_target) const {
  const Location *location = NULL;
  std::string     path;

  std::vector<const Location *>::const_iterator it = _location_vector_.begin();
  for (; it != _location_vector_.end(); ++it) {
    if (request_target.find((*it)->location_path_) == 0) {
      if (path.size() < (*it)->location_path_.size()) {
        path     = (*it)->location_path_;
        location = *it;
      }
    }
  }
  return location;
}