#include "util.hpp"
#include "Lexer.hpp"
#include <fstream>
#include <limits>
#include <sstream>

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

std::string sizet_to_string(std::size_t val) {
  const int max_digits = std::numeric_limits<std::size_t>::digits10 + 1;
  char      buffer[max_digits + 1];
  std::sprintf(buffer, "%zu", val);
  return buffer;
}

std::string read_file_to_string(const std::string &path) {
  std::ifstream file(path.c_str());
  if (!file.good()) {
    std::cout << "fail to open file" << std::endl;
    return "file not found"; // -> 404 or 500
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();
  return buffer.str();
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

bool is_digits(const std::string &token) {
  for (size_t i = 0; i < token.size(); i++) {
    if (!isdigit(token[i]))
      return false;
  }
  return true;
}
