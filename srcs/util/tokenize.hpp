#ifndef SRCS_TOKENIZE_HPP
#define SRCS_TOKENIZE_HPP

#include <string>
#include <vector>

std::vector<std::string> tokenize(const std::string &text,
                                  const std::string &delimiter,
                                  const std::string &skip);

#endif /* SRCS_TOKENIZE_HPP */
