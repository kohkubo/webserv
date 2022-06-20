#include "config/ConfigList.hpp"

#include "utils/file_io_utils.hpp"

ConfigList::ConfigList(const char *config_file_path) {
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
      _config_list_.push_back(config);
    } else {
      ERROR_EXIT("unexpected token: " << *it);
    }
  }
}