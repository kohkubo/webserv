#ifndef INCLUDES_RESPONSE_HPP
#define INCLUDES_RESPONSE_HPP

#include <iostream>
#include <vector>

class Response {
public:
  Response(const std::string &request_message);

  void        process();
  std::string message();

private:
  Response();
  Response(Response const &other);
  Response                &operator=(Response const &other);

  // リクエスト
  std::vector<std::string> __request_line_;
  std::vector<std::string> __request_field_;
  std::string              __request_body_;

  // レスポンス
  std::vector<std::string> __status_line_;
  std::vector<std::string> __response_field_;
  std::string              __response_body_;
};

#endif /* INCLUDES_RESPONSE_HPP */