#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <vector>

/*
 * リクエストとレスポンスの1行目に関して
 * indexをenumにすればわかりやすくなるかと思ったが,
 * VERSIONが被って面倒
 */
// enum RequestLine { METHOD, TARGET, VERSION };
// enum StatusLine { VERSION, STATUS, REASON };

class Response {
public:
  Response(const std::string &message);
  Response(Response const &other);
  Response   &operator=(Response const &other);

  void        method_get(std::string file_path);
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

#endif /* RESPONSE_HPP */