#ifndef SRCS_CONFIG_DIRECTIVE_PARSER_HPP
#define SRCS_CONFIG_DIRECTIVE_PARSER_HPP

#include <map>
#include <string>

#include "utils/Path.hpp"
#include "utils/tokenize.hpp"

tokenIterator parse_string_directive(std::string key, std::string &value,
                                     tokenIterator pos, tokenIterator end);

tokenIterator parse_path_directive(std::string key, Path &value,
                                   tokenIterator pos, tokenIterator end);

tokenIterator parse_size_directive(std::string key, size_t &value,
                                   tokenIterator pos, tokenIterator end);

tokenIterator parse_bool_directive(std::string key, bool &value,
                                   tokenIterator pos, tokenIterator end);

tokenIterator parse_vector_directive(std::string               key,
                                     std::vector<std::string> &value,
                                     tokenIterator pos, tokenIterator end);

tokenIterator parse_map_directive(std::string                 key,
                                  std::map<int, std::string> &value,
                                  tokenIterator pos, tokenIterator end);

#endif /* SRCS_CONFIG_DIRECTIVE_PARSER_HPP */
