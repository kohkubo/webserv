#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <vector>

/*
 * リクエストとレスポンスの1行目に関して
 * indexをenumにすればわかりやすくなるかと思ったが,
 * 'VERSION'が被って面倒
 */
// enum RequestLine { METHOD, TARGET, VERSION };
// enum StatusLine { VERSION, STATUS, REASON };

class Response {
public:
  Response(const std::string &message);
  Response(Response const &other);
  Response   &operator=(Response const &other);

  void        process();
  std::string response_message();

private:
  // リクエスト
  std::vector<std::string> __request_line_;
  std::vector<std::string> __request_field_;
  std::string              __request_body_;

  // レスポンス
  std::vector<std::string> __status_line_;
  std::vector<std::string> __response_field_;
  std::string              __response_body_;
};

std::string read_file(const std::string &path);
std::string sizettos(size_t val);

#endif /* RESPONSE_HPP */