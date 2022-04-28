#include <string>
#include <vector>

static bool is_delimiter(const char &c, const std::string &delimiter) {
  if (delimiter.find(c) != std::string::npos)
    return true;
  return false;
}

static bool is_skip(const char &c, const std::string &skip) {
  if (skip.find(c) != std::string::npos)
    return true;
  return false;
}

std::vector<std::string> tokenize(const std::string &text,
                                  const std::string &delimiter,
                                  const std::string &skip) {
  std::vector<std::string> res;
  std::string::size_type   pos = 0;
  while (text[pos]) {
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
