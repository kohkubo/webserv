#ifndef INCLUDES_LEXER_HPP
#define INCLUDES_LEXER_HPP

#include <string>
#include <vector>

class Lexer {
public:
  typedef std::vector<std::string>::iterator token_iterator;

private:
  std::vector<std::string> __token_;
  std::string              __delimiter_;

public:
  Lexer(const std::string text, const std::string &delimiter);
  Lexer(const Lexer &other);
  Lexer &operator=(const Lexer &other);
  ~Lexer() {}
  token_iterator begin() { return __token_.begin(); }
  token_iterator end() { return __token_.end(); }

private:
  Lexer() {}
  bool __tokenize(const std::string text);
  bool __is_delimiter(const char &c);
};

#endif /* INCLUDES_LEXER_HPP */
