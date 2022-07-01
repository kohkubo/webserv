#ifndef SRCS_CONFIG_CONFGROUPGEN_HPP
#define SRCS_CONFIG_CONFGROUPGEN_HPP

#include <vector>

#include "config/ConfigGroup.hpp"
#include "utils/Path.hpp"

std::vector<ConfigGroup> generate_config_group(const Path config_file_path);

#endif /* SRCS_CONFIG_CONFGROUPGEN_HPP */
