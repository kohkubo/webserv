#include "gtest/gtest.h"

#include "utils/tokenize.hpp"

#include <iostream>
#include <string>

#define SPACES "\v\r\f\t\n "

TEST(tokenize_test, simple_split) {
  tokenVector token_vector = tokenize("apple,orange,banana", ",", SPACES);
  tokenVector::iterator it = token_vector.begin();
  EXPECT_EQ(*it++, std::string("apple"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("orange"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("banana"));
  EXPECT_EQ(it, token_vector.end());
}

TEST(tokenize_test, no_delimiter) {
  tokenVector token_vector = tokenize("apple,orange,banana", " ", SPACES);
  tokenVector::iterator it = token_vector.begin();
  EXPECT_EQ(*it++, std::string("apple,orange,banana"));
  EXPECT_EQ(it, token_vector.end());
}

TEST(tokenize_test, multi_delimiter) {
  tokenVector token_vector = tokenize("apple, orange, banana", " ,", SPACES);
  tokenVector::iterator it = token_vector.begin();
  EXPECT_EQ(*it++, std::string("apple"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("orange"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("banana"));
  EXPECT_EQ(it, token_vector.end());
}

TEST(tokenize_test, skip_front_back) {
  tokenVector token_vector =
      tokenize("\n apple, orange, banana \n", " ,", SPACES);
  tokenVector::iterator it = token_vector.begin();
  EXPECT_EQ(*it++, std::string("apple"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("orange"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("banana"));
  EXPECT_EQ(it, token_vector.end());
}

TEST(tokenize_test, delimiter) {
  tokenVector           token_vector = tokenize("{{}}{}{,}", "{},", SPACES);
  tokenVector::iterator it           = token_vector.begin();
  EXPECT_EQ(*it++, std::string("{"));
  EXPECT_EQ(*it++, std::string("{"));
  EXPECT_EQ(*it++, std::string("}"));
  EXPECT_EQ(*it++, std::string("}"));
  EXPECT_EQ(*it++, std::string("{"));
  EXPECT_EQ(*it++, std::string("}"));
  EXPECT_EQ(*it++, std::string("{"));
  EXPECT_EQ(*it++, std::string(","));
  EXPECT_EQ(*it++, std::string("}"));
  EXPECT_EQ(it, token_vector.end());
}

TEST(tokenize_test, config) {
  std::string           str  = "server {\n"
                               "  listen 127.0.0.1:80;\n"
                               "  location /hoge {\n"
                               "    root /var/fuga;\n"
                               "  }\n"
                               "}\n";
  tokenVector           test = tokenize(str, SPACES "{};", SPACES);
  tokenVector::iterator it   = test.begin();
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

TEST(tokenize_test, httpReqest) {
  std::string           str  = "POST /search.html HTTP/1.1\r\n"
                               "Host: wa3.i-3-i.info\r\n"
                               "Connection: keep-alive\r\n"
                               "\r\n"
                               "body";

  tokenVector           test = tokenize(str, "\r\n:", " ");
  tokenVector::iterator it   = test.begin();
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
