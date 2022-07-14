#ifndef SRCS_CONFIG_DIRECTIVE_PARSER_HPP
#define SRCS_CONFIG_DIRECTIVE_PARSER_HPP

#include <map>
#include <string>

#include "utils/Path.hpp"
#include "utils/tokenize.hpp"

namespace config {

// clang-format off
tokenIterator parse_string_directive(std::string &value, tokenIterator pos, tokenIterator end);
tokenIterator parse_path_directive(Path &value, tokenIterator pos,tokenIterator end);
tokenIterator parse_size_directive(size_t &value, tokenIterator pos,tokenIterator end);
tokenIterator parse_bool_directive(bool &value,tokenIterator pos, tokenIterator end);
tokenIterator parse_vector_directive(std::vector<std::string> &value, tokenIterator pos, tokenIterator end);
tokenIterator parse_map_directive(std::map<int, std::string> &value, tokenIterator pos, tokenIterator end);

// clang-format on

} // namespace config

#endif /* SRCS_CONFIG_DIRECTIVE_PARSER_HPP */
