#ifndef INCLUDES_LEXER_LEXER
#define INCLUDES_LEXER_LEXER

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

public:
  static Lexer::token_iterator skip_delimiter(Lexer::token_iterator pos,
                                              Lexer::token_iterator end,
                                              const std::string    &skip);
};

#endif /* INCLUDES_LEXER_LEXER */
