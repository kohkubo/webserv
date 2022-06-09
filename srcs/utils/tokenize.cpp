#include "tokenize.hpp"

#include <string>
#include <vector>

static bool is_delimiter(const char &c, const std::string &delimiter) {
  return delimiter.find(c) != std::string::npos;
}

static bool is_skip(const char &c, const std::string &skip) {
  return skip.find(c) != std::string::npos;
}

tokenVector tokenize(const std::string &text, const std::string &delimiter,
                     const std::string &skip) {
  tokenVector            res;
  std::string::size_type pos = 0;
  while (text[pos] != '\0') {
    std::string::size_type i = 0;
    if (is_delimiter(text[pos], delimiter)) {
      i = 1;
    } else {
      while (!is_delimiter(text[pos + i], delimiter) && pos + i < text.size()) {
        i++;
      }
    }
    if (!(is_skip(text[pos], skip) && i == 1))
      res.push_back(text.substr(pos, i));
    pos += i;
  }
  return res;
}
