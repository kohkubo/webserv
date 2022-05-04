#ifndef SRCS_TOKENIZE_HPP
#define SRCS_TOKENIZE_HPP

#include <string>
#include <vector>

typedef std::vector<std::string> token_vector;
typedef token_vector::iterator   token_iterator;

token_vector tokenize(const std::string &text, const std::string &delimiter,
                      const std::string &skip);

#endif /* SRCS_TOKENIZE_HPP */
