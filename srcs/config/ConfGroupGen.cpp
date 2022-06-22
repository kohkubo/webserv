#include "config/ConfGroupGen.hpp"

#include "utils/file_io_utils.hpp"

static std::vector<ConfigGroup>::iterator
find_joinable_config_group(const Config             &config,
                           std::vector<ConfigGroup> &config_groups) {
  std::vector<ConfigGroup>::iterator it = config_groups.begin();
  for (; it != config_groups.end(); it++) {
    if (it->is_same_socket(config))
      break;
  }
  return it;
}

std::vector<ConfigGroup> generate_config_group(const char *config_file_path) {
  std::vector<ConfigGroup> config_groups;

  Result<std::string> result = read_file_to_str(config_file_path);
  if (result.is_err_) {
    ERROR_EXIT(config_file_path << " is not found or can't read.");
  }
  tokenVector token_vector =
      tokenize(result.object_, CONFIG_DELIMITER, CONFIG_SKIP);
  tokenIterator it = token_vector.begin();
  while (it != token_vector.end()) {
    if (*it == "server") {
      Config config(it, token_vector.end());
      it = config.last_iterator_pos();
      std::vector<ConfigGroup>::iterator joinable_config_group =
          find_joinable_config_group(config, config_groups);
      if (joinable_config_group != config_groups.end()) {
        joinable_config_group->add_config_or_exit(config);
      } else {
        config_groups.push_back(ConfigGroup(config));
      }
    } else {
      ERROR_EXIT("unexpected token: " << *it);
    }
  }

  return config_groups;
}