#ifndef SRCS_CONFIG_LOCATIONS_HPP
#define SRCS_CONFIG_LOCATIONS_HPP

#include <vector>

#include "config/Location.hpp"

namespace config {

class Locations {
private:
  std::vector<Location> _location_vector_;

private:
  Locations(Locations &other);

public:
  Locations() {}
  ~Locations(){};
  void            add_or_exit(const Location &location);
  const Location *select_location(const Path &request_target) const;
  bool            empty() const { return _location_vector_.empty(); }
};

} // namespace config

#endif /* SRCS_CONFIG_LOCATIONS_HPP */
