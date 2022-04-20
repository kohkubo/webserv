#include "gtest/gtest.h"

#include "Tokens.hpp"

#include <iostream>
#include <string>

#define SPACES "\v\r\f\t\n "

TEST(tokens_test, simple_split) {
  Tokens                 l("apple,orange,banana", ",", SPACES);
  Tokens::token_iterator it = l.begin();
  EXPECT_EQ(*it++, std::string("apple"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("orange"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("banana"));
  EXPECT_EQ(it, l.end());
}

TEST(tokens_test, no_delimiter) {
  Tokens                 l("apple,orange,banana", " ", SPACES);
  Tokens::token_iterator it = l.begin();
  EXPECT_EQ(*it++, std::string("apple,orange,banana"));
  EXPECT_EQ(it, l.end());
}

TEST(tokens_test, multi_delimiter) {
  Tokens                 l("apple, orange, banana", " ,", SPACES);
  Tokens::token_iterator it = l.begin();
  EXPECT_EQ(*it++, std::string("apple"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("orange"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("banana"));
  EXPECT_EQ(it, l.end());
}

TEST(tokens_test, delimiter) {
  Tokens                 l("{{}}{}{,}", "{},", SPACES);
  Tokens::token_iterator it = l.begin();
  EXPECT_EQ(*it++, std::string("{"));
  EXPECT_EQ(*it++, std::string("{"));
  EXPECT_EQ(*it++, std::string("}"));
  EXPECT_EQ(*it++, std::string("}"));
  EXPECT_EQ(*it++, std::string("{"));
  EXPECT_EQ(*it++, std::string("}"));
  EXPECT_EQ(*it++, std::string("{"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("}"));
  EXPECT_EQ(it, l.end());
}

TEST(tokens_test, config) {
  std::string            str = R"(server {
  listen 127.0.0.1:80;
  location /hoge {
    root /var/fuga;
  }
})";
  Tokens                 test(str, SPACES "{};", SPACES);
  Tokens::token_iterator it = test.begin();
  EXPECT_EQ(*it++, std::string("server"));
  EXPECT_EQ(*it++, std::string("{"));
  EXPECT_EQ(*it++, std::string("listen"));
  EXPECT_EQ(*it++, std::string("127.0.0.1:80"));
  EXPECT_EQ(*it++, std::string(";"));
  EXPECT_EQ(*it++, std::string("location"));
  EXPECT_EQ(*it++, std::string("/hoge"));
  EXPECT_EQ(*it++, std::string("{"));
  EXPECT_EQ(*it++, std::string("root"));
  EXPECT_EQ(*it++, std::string("/var/fuga"));
  EXPECT_EQ(*it++, std::string(";"));
  EXPECT_EQ(*it++, std::string("}"));
  EXPECT_EQ(*it++, std::string("}"));
  EXPECT_EQ(it, test.end());
}

TEST(tokens_test, httpReqest) {
  std::string            str = "POST /search.html HTTP/1.1\r\n"
                               "Host: wa3.i-3-i.info\r\n"
                               "Connection: keep-alive\r\n"
                               "\r\n"
                               "body";

  Tokens                 test(str, "\r\n:", " ");
  Tokens::token_iterator it = test.begin();
  EXPECT_EQ(*it++, std::string("POST /search.html HTTP/1.1"));
  EXPECT_EQ(*it++, std::string("\r"));
  EXPECT_EQ(*it++, std::string("\n"));
  EXPECT_EQ(*it++, std::string("Host"));
  EXPECT_EQ(*it++, std::string(":"));
  EXPECT_EQ(*it++, " wa3.i-3-i.info");
  EXPECT_EQ(*it++, std::string("\r"));
  EXPECT_EQ(*it++, std::string("\n"));
  EXPECT_EQ(*it++, std::string("Connection"));
  EXPECT_EQ(*it++, std::string(":"));
  EXPECT_EQ(*it++, std::string(" keep-alive"));
  EXPECT_EQ(*it++, std::string("\r"));
  EXPECT_EQ(*it++, std::string("\n"));
  EXPECT_EQ(*it++, std::string("\r"));
  EXPECT_EQ(*it++, std::string("\n"));
  EXPECT_EQ(*it++, std::string("body"));
  EXPECT_EQ(it, test.end());
}
