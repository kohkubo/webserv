#include "utils/convert_code.hpp"

//#include <stdio.h>
#include <iomanip>
#include <sstream>

#include <iostream>
// どの文字をエンコードするのかの定義はmdn?
// utf8はこの後の話？
// 変換時のcharとかintとかわからん

// cgiはpipe[1]から読み込んでpipe[1]に書き込む,
// pipe[1]から読み込む際にそこに元々書き込む部分はどこで?

// itestのmultiが分からなかった, 405と403のちがいも
// has_availablemethodのフラグはいる？

static bool is_encode(const char c) { return (c == '%'); }

static std::string char_to_hexadecimal_string(const char c) {
  std::ostringstream oss;
  oss << std::hex << std::setfill('0') << std::setw(2) << std::uppercase
      << (c & 0x000000FF);
  return oss.str();
}
// char dest[100];
// int  len = sprintf(dest, "%%%02X", str.at(pos) & 0x000000FF);

// static_cast<unsigned int>(static_cast<unsigned char>

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
