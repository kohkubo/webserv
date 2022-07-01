#ifndef SRCS_CONFIG_LOCATIONVECTOR_HPP
#define SRCS_CONFIG_LOCATIONVECTOR_HPP

#include <vector>

#include "config/Location.hpp"

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

#endif /* SRCS_CONFIG_LOCATIONVECTOR_HPP */
