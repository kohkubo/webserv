#include "utils/convert_code.hpp"

static bool is_encode(const char c) { return (c == '%' || c == ':'); }

static std::string char_to_hexadecimal_string(const char c) {
  std::ostringstream oss;
  oss << std::hex << std::setfill('0') << std::setw(2) << std::uppercase
      << static_cast<unsigned int>(static_cast<unsigned char>(c));
  return oss.str();
}
// char dest[100];
// int  len = sprintf(dest, "%%%02X", str.at(pos) & 0x000000FF);

std::string percent_encode(std::string str) {
  std::string ret;
  for (std::size_t pos = 0; pos < str.length(); pos++) {
    if (is_encode(str.at(pos))) {
      ret += "%" + char_to_hexadecimal_string(str.at(pos));
    } else {
      ret.push_back(str.at(pos));
    }
  }
  return ret;
}
