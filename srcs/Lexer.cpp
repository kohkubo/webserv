#include "Lexer.hpp"

#include <iostream>

Lexer::Lexer(const std::string text, const std::string &delimiter)
    : __delimiter_(delimiter) {
  if (!__tokenize(text)) {
    // TODO: error handling
    std::cerr << "something wrong." << std::endl;
  }
}

Lexer::Lexer(const Lexer &other)
    : __token_(other.__token_), __delimiter_(other.__delimiter_) {}

Lexer &Lexer::operator=(const Lexer &other) {
  if (this != &other) {
    __token_     = other.__token_;
    __delimiter_ = other.__delimiter_;
  }
  return *this;
}

bool Lexer::__is_delimiter(const char &c) {
  if (__delimiter_.find(c) != std::string::npos)
    return true;
  return false;
}

bool Lexer::__tokenize(const std::string text) {
  std::string::size_type pos = 0;
  while (text[pos]) {
    std::string::size_type i = 0;
    if (__is_delimiter(text[pos])) {
      i = 1;
    } else {
      while (!__is_delimiter(text[pos + i]) && pos + i < text.size()) {
        i++;
      }
    }
    __token_.push_back(text.substr(pos, i));
    pos += i;
  }
  return true;
}

Lexer::token_iterator Lexer::skip_delimiter(Lexer::token_iterator pos,
                                            Lexer::token_iterator end,
                                            const std::string &   skip) {
  while (pos != end) {
    if (skip.find(pos[0]) == std::string::npos)
      break;
    pos++;
  }
  return pos;
}
