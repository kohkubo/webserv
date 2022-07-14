#include "config/Location.hpp"

#include "config/directive_parser.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

namespace config {

bool Location::is_unavailable_method(const std::string &method) const {
  if (!has_available_methods_) {
    return false;
  }
  if (method == "GET") {
    return !available_methods_.get_;
  }
  if (method == "POST") {
    return !available_methods_.post_;
  }
  if (method == "GET") {
    return !available_methods_.delete_;
  }
  return true;
}

} // namespace config