#include "util.hpp"
#include "Lexer.hpp"

bool is_match_suffix_string(const std::string &str, const std::string &suffix) {
  if (str.length() < suffix.length()) {
    return false;
  }
  if (str.compare(str.length() - suffix.length(), suffix.length(), suffix) ==
      0) {
    return true;
  }
  return false;
}

bool is_octet(const std::string &token) {
  if (token.size() == 1) {
    if (token[0] >= '0' && token[0] <= '9') {
      return true;
    }
    return false;
  } else if (token.size() == 2) {
    if (token[0] >= '1' && token[0] <= '9' && token[1] >= '0' &&
        token[1] <= '9') {
      return true;
    }
    return false;
  } else if (token.size() == 3) {
    if (token[0] == '1') {
      if ((token[1] >= '0' && token[1] <= '9') &&
          (token[2] >= '0' && token[2] <= '9'))
        return true;
    }
    if (token[0] == '2') {
      if ((token[1] >= '0' && token[1] <= '4') &&
          (token[2] >= '0' && token[2] <= '9'))
        return true;
      if (token[1] == '5' && (token[2] >= '0' && token[2] <= '5'))
        return true;
    }
  }
  return false;
}

bool is_ip(const std::string &token) {
  Lexer                 l(token, ".");
  Lexer::token_iterator it    = l.begin();
  size_t                count = 0;
  for (; it != l.end(); it++, count++) {
    if (!is_octet(*it))
      return false;
    it++;
    if (count == 3)
      break;
    if (it != l.end() && *it != ".")
      return false;
  }
  return true;
}
