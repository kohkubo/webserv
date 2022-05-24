#include "gtest/gtest.h"

#include <string>

#include "http/request/RequestInfo.hpp"

TEST(request_parse_test, normal) {
  std::string str = "GET / HTTP/1.1\r\n"
                    "Host: 127.0.0.1:5001\r\n"
                    "User-Agent: curl/7.68.0\r\n"
                    "Connection: close\r\n"
                    "Accept: */*\r\n";

  RequestInfo info;
  info.parse_request_header(str);
  EXPECT_EQ(info.method_, GET);
  EXPECT_EQ(info.uri_, "/");
  EXPECT_EQ(info.version_, "HTTP/1.1");
  EXPECT_EQ(info.host_, "127.0.0.1");
  EXPECT_EQ(info.port_, "5001");
  EXPECT_EQ(info.is_close_, true);
  EXPECT_EQ(info.get_field_value("User-Agent"), "curl/7.68.0");
  EXPECT_EQ(info.get_field_value("Accept"), "*/*");
}

TEST(request_parse_test, normal_delete) {
  std::string str = "DELETE /delete_target.tmp HTTP/1.1\r\n"
                    "Host: 127.0.0.1:5001\r\n"
                    "User-Agent: curl/7.68.0\r\n"
                    "Connection: close\r\n"
                    "Accept: */*\r\n";

  RequestInfo info;
  info.parse_request_header(str);
  EXPECT_EQ(info.method_, DELETE);
  EXPECT_EQ(info.uri_, "/delete_target.tmp");
  EXPECT_EQ(info.version_, "HTTP/1.1");
  EXPECT_EQ(info.host_, "127.0.0.1");
  EXPECT_EQ(info.port_, "5001");
  EXPECT_EQ(info.is_close_, true);
  EXPECT_EQ(info.get_field_value("User-Agent"), "curl/7.68.0");
  EXPECT_EQ(info.get_field_value("Accept"), "*/*");
}

TEST(request_parse_test, normal_post) {
  std::string str = "POST /target HTTP/1.1\r\n"
                    "Host: 127.0.0.1:5001\r\n"
                    "User-Agent: curl/7.68.0\r\n"
                    "Connection: close\r\n"
                    "Accept: */*\r\n"
                    "Content-Type: application/x-www-form-urlencoded\r\n"
                    "Content-Length: 18\r\n";

  RequestInfo info;
  info.parse_request_header(str);
  EXPECT_EQ(info.method_, POST);
  EXPECT_EQ(info.uri_, "/target");
  EXPECT_EQ(info.version_, "HTTP/1.1");
  EXPECT_EQ(info.host_, "127.0.0.1");
  EXPECT_EQ(info.port_, "5001");
  EXPECT_EQ(info.is_close_, true);
  EXPECT_EQ(info.content_length_, 18);
  EXPECT_EQ(info.get_field_value("User-Agent"), "curl/7.68.0");
  EXPECT_EQ(info.get_field_value("Accept"), "*/*");
  EXPECT_EQ(info.get_field_value("Content-Type"),
            "application/x-www-form-urlencoded");
}

TEST(request_parse_test, query_body) {
  std::string header = "POST /target HTTP/1.1\r\n"
                       "Host: 127.0.0.1:5001\r\n"
                       "Content-Type: application/x-www-form-urlencoded\r\n"
                       "Content-Length: 45\r\n";

  std::string body   = "I'm=going"
                       "&to=become"
                       "&the=king"
                       "&of=the"
                       "&pirates!!";
  RequestInfo info;
  info.parse_request_header(header);
  info.parse_request_body(body);
  EXPECT_EQ(info.values_[0], "I'm=going");
  EXPECT_EQ(info.values_[1], "to=become");
  EXPECT_EQ(info.values_[2], "the=king");
  EXPECT_EQ(info.values_[3], "of=the");
  EXPECT_EQ(info.values_[4], "pirates!!");
  // 今は"hoge=huga"の形でなくてもバリデートしてない
}

TEST(request_parse_test, query_body_capital) {
  std::string header = "POST /target HTTP/1.1\r\n"
                       "Host: 127.0.0.1:5001\r\n"
                       "Content-Type: AppliCation/x-WWW-form-URLENCODED\r\n"
                       "Content-Length: 45\r\n";

  std::string body   = "yabu=kara"
                       "&stick="
                       "&ishi=no"
                       "&uenimo=3years";
  RequestInfo info;
  info.parse_request_header(header);
  info.parse_request_body(body);
  EXPECT_EQ(info.values_[0], "yabu=kara");
  EXPECT_EQ(info.values_[1], "stick=");
  EXPECT_EQ(info.values_[2], "ishi=no");
  EXPECT_EQ(info.values_[3], "uenimo=3years");
}

TEST(request_parse_test, exception_request_line_few_field) {
  std::string str = "GET HTTP/1.1\r\n"
                    "Host: 127.0.0.1:5001\r\n";

  RequestInfo info;
  EXPECT_THROW(info.parse_request_header(str),
               RequestInfo::BadRequestException);
}

TEST(request_parse_test, exception_request_line_no_space) {
  std::string str = "GET/HTTP/1.1\r\n"
                    "Host: 127.0.0.1:5001\r\n";

  RequestInfo info;
  EXPECT_THROW(info.parse_request_header(str),
               RequestInfo::BadRequestException);
}

TEST(request_parse_test, exception_field_name_space) {
  std::string str = "GET / HTTP/1.1\r\n"
                    "Host : 127.0.0.1:5001\r\n";

  RequestInfo info;
  EXPECT_THROW(info.parse_request_header(str),
               RequestInfo::BadRequestException);
}

TEST(request_parse_test, exception_field_name_tab) {
  std::string str = "GET / HTTP/1.1\r\n"
                    "Host\t: 127.0.0.1:5001\r\n";

  RequestInfo info;
  EXPECT_THROW(info.parse_request_header(str),
               RequestInfo::BadRequestException);
}

TEST(request_parse_test, exception_no_host) {
  std::string str = "GET / HTTP/1.1\r\n"
                    "Connection: close\r\n"
                    "Accept: */*\r\n";

  RequestInfo info;
  EXPECT_THROW(info.parse_request_header(str),
               RequestInfo::BadRequestException);
}

TEST(request_parse_test, exception_host_range) {
  std::string str = "GET / HTTP/1.1\r\n"
                    "Host: 127.0.0.1:99999\r\n";

  RequestInfo info;
  EXPECT_THROW(info.parse_request_header(str),
               RequestInfo::BadRequestException);
}
