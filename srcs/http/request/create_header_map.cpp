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

// request-line = method SP request-target SP HTTP-version (CRLF)
static void parse_request_line(const std::string &request_line) {
  std::size_t first_sp = request_line.find_first_of(' ');
  std::size_t last_sp  = request_line.find_last_of(' ');
  // TODO: check npos
  std::string method   = request_line.substr(0, first_sp);
  std::string target   = request_line.substr(first_sp + 1, last_sp);
  std::string version  = request_line.substr(last_sp + 1);
  // parse_method();
  // parse_target();
  // parse_version();
}

static bool is_comma_sparated(std::string &field_name) {
  (void)field_name;
  // カンマ区切りが定義されたフィールドか判定する。
  // tmp
  return true;
}

// エラーをエスカレートする必要あり
std::map<std::string, std::string> create_header_map(token_iterator it,
                                                     token_iterator end) {
  std::map<std::string, std::string> res;
  for (; it != end; it++) {
    std::string line = *it;
    std::size_t pos  = line.find(':');
    if (pos == std::string::npos) {
      // error bad request
    }
    std::string field_name = line.substr(0, pos);
    char        last_char  = field_name[field_name.size() - 1];
    if (last_char == ' ' || last_char == '\t') {
      // bad request
    }
    std::string field_value = trim_space(line.substr(pos + 1));
    if (res.count(field_name)) {
      if (is_comma_sparated(field_name)) {
        res[field_name] += ", " + field_value;
      } else {
        // bad request
      }
    } else {
      res[field_name] = field_value;
    }
  }
  return res;
}

void parse_request_header(const std::string &request_string) {
  token_vector   fields = tokenize(request_string, CRLF, CRLF);
  token_iterator it     = fields.begin();

  parse_request_line(*it++);
  std::map<std::string, std::string> header_map =
      create_header_map(it, fields.end());
  // call each field's parser
}