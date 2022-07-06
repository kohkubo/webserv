#ifndef SRCS_CONFIG_CONFGROUPGEN_HPP
#define SRCS_CONFIG_CONFGROUPGEN_HPP

#include <vector>

#include "config/ConfigGroup.hpp"
#include "utils/Path.hpp"

namespace config {

std::vector<ConfigGroup> generate_config_group(const Path config_file_path);

} // namespace config

#endif /* SRCS_CONFIG_CONFGROUPGEN_HPP */
