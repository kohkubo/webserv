#include "gtest/gtest.h"

#include "Lexer.hpp"

#include <iostream>
#include <string>

#define SPACES "\v\r\f\t\n "

TEST(lexer_test, simple_split) {
  Lexer                 l("apple,orange,banana", ",");
  Lexer::token_iterator it = l.begin();
  EXPECT_EQ(*it++, std::string("apple"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("orange"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("banana"));
  EXPECT_EQ(it, l.end());
}

TEST(lexer_test, no_delimiter) {
  Lexer                 l("apple,orange,banana", " ");
  Lexer::token_iterator it = l.begin();
  EXPECT_EQ(*it++, std::string("apple,orange,banana"));
  EXPECT_EQ(it, l.end());
}

TEST(lexer_test, multi_delimiter) {
  Lexer                 l("apple, orange, banana", " ,");
  Lexer::token_iterator it = l.begin();
  EXPECT_EQ(*it++, std::string("apple"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string("orange"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string("banana"));
  EXPECT_EQ(it, l.end());
}

TEST(lexer_test, delimiter) {
  Lexer                 l("{{}}{}{,}", "{},");
  Lexer::token_iterator it = l.begin();
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

TEST(lexer_test, config) {
  std::string           str = R"(server {
  listen 127.0.0.1:80;
  location /hoge {
    root /var/fuga;
  }
})";
  Lexer                 test(str, SPACES "{};");
  Lexer::token_iterator it = test.begin();
  EXPECT_EQ(*it++, std::string("server"));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string("{"));
  EXPECT_EQ(*it++, std::string("\n"));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string("listen"));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string("127.0.0.1:80"));
  EXPECT_EQ(*it++, std::string(";"));
  EXPECT_EQ(*it++, std::string("\n"));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string("location"));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string("/hoge"));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string("{"));
  EXPECT_EQ(*it++, std::string("\n"));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string("root"));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string("/var/fuga"));
  EXPECT_EQ(*it++, std::string(";"));
  EXPECT_EQ(*it++, std::string("\n"));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string(" "));
  EXPECT_EQ(*it++, std::string("}"));
  EXPECT_EQ(*it++, std::string("\n"));
  EXPECT_EQ(*it++, std::string("}"));
  EXPECT_EQ(it, test.end());
}

TEST(lexer_test, httpReqest) {
  std::string str = "POST /search.html HTTP/1.1\r\n"
                    "Host: wa3.i-3-i.info\r\n"
                    "Connection: keep-alive\r\n"
                    "\r\n"
                    "body";

  Lexer                 test(str, "\r\n:");
  Lexer::token_iterator it = test.begin();
  EXPECT_EQ(*it++, std::string("POST /search.html HTTP/1.1"));
  EXPECT_EQ(*it++, std::string("\r"));
  EXPECT_EQ(*it++, std::string("\n"));
  EXPECT_EQ(*it++, std::string("Host"));
  EXPECT_EQ(*it++, std::string(":"));
  EXPECT_EQ(*it++, std::string(" wa3.i-3-i.info"));
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