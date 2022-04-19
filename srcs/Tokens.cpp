
#include "Tokens.hpp"

Tokens::Tokens(const std::string text, const std::string &delimiter,
               const std::string &skip)
    : __delimiter_(delimiter), __skip_(skip) {
  __tokenize(text);
}

Tokens::Tokens(const Tokens &other)
    : __delimiter_(other.__delimiter_), __skip_(other.__skip_) {}

Tokens &Tokens::operator=(const Tokens &other) {
  if (this != &other) {
    __delimiter_ = other.__delimiter_;
    __skip_      = other.__skip_;
  }
  return *this;
}

bool Tokens::__tokenize(const std::string &text) {
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
    push_back(text.substr(pos, i));
    pos += i;
  }
  return true;
}

bool Tokens::__is_delimiter(const char &c) {
  if (__delimiter_.find(c) != std::string::npos)
    return true;
  return false;
}