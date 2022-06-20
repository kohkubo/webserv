#include "config/ConfGroupGen.hpp"

#include "utils/file_io_utils.hpp"

static std::vector<ConfigGroup>::iterator
find_same_socket(const Config             &config,
                 std::vector<ConfigGroup> &config_group_list) {
  std::vector<ConfigGroup>::iterator it = config_group_list.begin();
  for (; it != config_group_list.end(); it++) {
    if (it->is_same_socket(config))
      break;
  }
  return it;
}

std::vector<ConfigGroup> generate_config_group(const char *config_file_path) {
  std::vector<ConfigGroup> config_group_list;

  Result result = read_file_to_str(config_file_path);
  if (result.is_err_) {
    ERROR_EXIT(config_file_path << " is not found or can't read.");
  }
  tokenVector token_vector =
      tokenize(result.str_, CONFIG_DELIMITER, CONFIG_SKIP);
  tokenIterator it = token_vector.begin();
  while (it != token_vector.end()) {
    if (*it == "server") {
      Config config(it, token_vector.end());
      it = config.last_iterator_pos();
      std::vector<ConfigGroup>::iterator same_socket_group =
          find_same_socket(config, config_group_list);
      if (same_socket_group != config_group_list.end()) {
        same_socket_group->try_add_config(config);
      } else {
        config_group_list.push_back(ConfigGroup(config));
      }
    } else {
      ERROR_EXIT("unexpected token: " << *it);
    }
  }

  return config_group_list;
}
