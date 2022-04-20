#ifndef INCLUDES_TOKENS_HPP
#define INCLUDES_TOKENS_HPP

#include <string>
#include <vector>

#include "TokenIterator.hpp"

class Tokens : public std::vector<std::string> {
public:
  typedef std::vector<std::string>           token_type;
  typedef std::vector<std::string>::iterator token_iterator;

private:
  std::string __delimiter_;
  std::string __skip_;

public:
  Tokens(const std::string &text, const std::string &delimiter,
         const std::string &skip);
  Tokens(const Tokens &other);
  Tokens &operator=(const Tokens &other);
  ~Tokens() {}

private:
  Tokens() {}
  bool __tokenize(const std::string &text);
  bool __is_delimiter(const char &c);
  bool __is_skip(const char &c);
};

#endif /* INCLUDES_TOKENS_HPP */
