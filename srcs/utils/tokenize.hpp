#ifndef SRCS_UTILS_TOKENIZE_HPP
#define SRCS_UTILS_TOKENIZE_HPP

#include <string>
#include <vector>

typedef std::vector<std::string> tokenVector;
typedef tokenVector::iterator    tokenIterator;

tokenVector tokenize(const std::string &text, const std::string &delimiter,
                     const std::string &skip);

#endif /* SRCS_UTILS_TOKENIZE_HPP */
