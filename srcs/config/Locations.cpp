#include "config/Locations.hpp"

#include <algorithm>
#include <string>

#include "utils/utils.hpp"

namespace config {

void Locations::add_or_exit(const Location &location) {
  Path                            location_path = location.location_path_.str();
  std::vector<Location>::iterator it            = _location_vector_.begin();
  for (; it != _location_vector_.end(); ++it) {
    if (it->location_path_ == location_path) {
      ERROR_EXIT("duplicate location directive.");
    }
  }
  _location_vector_.push_back(location);
}

const Location *Locations::select_location(const Path &request_target) const {
  const Location *location = NULL;
  std::string     path;

  std::vector<Location>::const_iterator it = _location_vector_.begin();
  for (; it != _location_vector_.end(); ++it) {
    if (request_target.str().find(it->location_path_.str()) == 0) {
      if (path.size() < it->location_path_.str().size()) {
        path     = it->location_path_.str();
        location = &(*it);
      }
    }
  }
  return location;
}

} // namespace config
