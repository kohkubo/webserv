#include "http.hpp"
#include "gtest/gtest.h"

// GET / HTTP/1.1
// Host: localhost:5001
// User-Agent: curl/7.79.1
// Accept: */*

TEST(response_message_test, simple_message) {
  http_message_map      request_message;
  request_message[METHOD] = "GET";
  request_message[URL] = "./../html/index.html";
  request_message[VERSION] = "HTTP/1.1";
  request_message[HOST] = "localhost:5001";
  request_message[USERAGENT] = "curl/7.79.1";
  request_message[ACCEPT] = "*/*";

  // フィールドの順番はfield_message()による
  std::string expect             = "HTTP/1.1 200 OK\r\n"
                                   "Content-Length: 4\r\n"
                                   "Content-Type: text/html\r\n"
                                   "Connection: close\r\n"
                                   "\r\n"
                                   "test";

  EXPECT_EQ(create_response(request_message), expect);
}

// response_message()がエラーを出すようになったらそれもテスト
