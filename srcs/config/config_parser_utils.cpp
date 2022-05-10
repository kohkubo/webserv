#include "utils/tokenize.hpp"
#include <string>

bool is_uint8(const std::string &token) {
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

bool is_ip(const std::string &field_value) {
  token_vector   tokens = tokenize(field_value, ".", "");
  token_iterator it     = tokens.begin();
  size_t         count  = 0;
  for (; it != tokens.end(); it++, count++) {
    if (!is_uint8(*it))
      return false;
    it++;
    if (count == 3)
      break;
    if (it != tokens.end() && *it != ".")
      return false;
  }
  return true;
}

bool is_digits(const std::string &field_value) {
  for (size_t i = 0; i < field_value.size(); i++) {
    if (!isdigit(field_value[i]))
      return false;
  }
  return true;
}
