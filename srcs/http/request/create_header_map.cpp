#include <map>
#include <string>

#include "http/HttpMessage.hpp"
#include "http/const/const_delimiter.hpp"
#include "utils/tokenize.hpp"

// TODO: check message/http media type (RFC7230 Section 8.3.1)

static std::string trim_space(std::string str) {
  str.erase(0, str.find_first_not_of(" \t"));
  str.erase(str.find_last_not_of(" \t") + 1);
  return str;
}

static bool is_comma_sparated(std::string &field_name) {
  // カンマ区切りが定義されたフィールドか判定する。
  // tmp
  return true;
}

// エラーをエスカレートする必要あり
std::map<std::string, std::string>
create_header_map(const std::string &request_string) {
  token_vector fields = tokenize(request_string, CRLF, CRLF);

  std::map<std::string, std::string> header_map;
  for (token_iterator it = fields.begin(); it != fields.end(); it++) {
    std::string line = *it;
    std::size_t pos  = line.find(':');
    if (pos == std::string::npos) {
      // error bad request
    }
    std::string field_name = line.substr(0, pos);
    if (field_name.back() == ' ' || field_name.back() == '\t') {
      // bad request
    }
    std::string field_value = trim_space(line.substr(pos + 1));
    if (header_map.count(field_name)) {
      if (is_comma_sparated(field_name)) {
        header_map[field_name] += ", " + field_value;
      } else {
        // bad request
      }
    } else {
      header_map[field_name] = field_value;
    }
  }
  return header_map;
}
