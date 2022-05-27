#include "gtest/gtest.h"

#include <string>

#include "event/Transaction.hpp"
#include "http/request/RequestInfo.hpp"

TEST(request_parse_test, normal) {
  std::string request = "GET / HTTP/1.1\r\n"
                        "Host: 127.0.0.1:5001\r\n"
                        "User-Agent: curl/7.68.0\r\n"
                        "Connection: close\r\n"
                        "Accept: */*\r\n\r\n";

  Transaction t;
  // tmp
  confGroup   dummy_group;
  Config      dummy_conf = Config();
  dummy_group.push_back(&dummy_conf);

  t.parse_single_request(request, dummy_group);
  RequestInfo &r = t.get_request_info();

  EXPECT_EQ(r.method_, GET);
  EXPECT_EQ(r.uri_, "/");
  EXPECT_EQ(r.version_, "HTTP/1.1");
  EXPECT_EQ(r.host_, "127.0.0.1");
  EXPECT_EQ(r.port_, "5001");
  EXPECT_EQ(r.is_close_, true);
  EXPECT_EQ(r.get_field_value("User-Agent"), "curl/7.68.0");
  EXPECT_EQ(r.get_field_value("Accept"), "*/*");
}

TEST(request_parse_test, normal_delete) {
  std::string request = "DELETE /delete_target.tmp HTTP/1.1\r\n"
                        "Host: 127.0.0.1:5001\r\n"
                        "User-Agent: curl/7.68.0\r\n"
                        "Connection: close\r\n"
                        "Accept: */*\r\n\r\n";

  Transaction t;
  // tmp
  confGroup   dummy_group;
  Config      dummy_conf = Config();
  dummy_group.push_back(&dummy_conf);

  t.parse_single_request(request, dummy_group);
  RequestInfo &r = t.get_request_info();

  EXPECT_EQ(r.method_, DELETE);
  EXPECT_EQ(r.uri_, "/delete_target.tmp");
  EXPECT_EQ(r.version_, "HTTP/1.1");
  EXPECT_EQ(r.host_, "127.0.0.1");
  EXPECT_EQ(r.port_, "5001");
  EXPECT_EQ(r.is_close_, true);
  EXPECT_EQ(r.get_field_value("User-Agent"), "curl/7.68.0");
  EXPECT_EQ(r.get_field_value("Accept"), "*/*");
}

TEST(request_parse_test, normal_post) {
  std::string request = "POST /target HTTP/1.1\r\n"
                        "Host: 127.0.0.1:5001\r\n"
                        "User-Agent: curl/7.68.0\r\n"
                        "Connection: close\r\n"
                        "Accept: */*\r\n"
                        "Content-Type: application/x-www-form-urlencoded\r\n"
                        "Content-Length: 18\r\n\r\n";

  Transaction t;
  // tmp
  confGroup   dummy_group;
  Config      dummy_conf = Config();
  dummy_group.push_back(&dummy_conf);

  t.parse_single_request(request, dummy_group);
  RequestInfo &r = t.get_request_info();

  EXPECT_EQ(r.method_, POST);
  EXPECT_EQ(r.uri_, "/target");
  EXPECT_EQ(r.version_, "HTTP/1.1");
  EXPECT_EQ(r.host_, "127.0.0.1");
  EXPECT_EQ(r.port_, "5001");
  EXPECT_EQ(r.is_close_, true);
  EXPECT_EQ(r.content_length_, 18);
  EXPECT_EQ(r.get_field_value("User-Agent"), "curl/7.68.0");
  EXPECT_EQ(r.get_field_value("Accept"), "*/*");
  EXPECT_EQ(r.get_field_value("Content-Type"),
            "application/x-www-form-urlencoded");
}

TEST(request_parse_test, query_body) {
  std::string request = "POST /target HTTP/1.1\r\n"
                        "Host: 127.0.0.1:5001\r\n"
                        "Content-Type: application/x-www-form-urlencoded\r\n"
                        "Content-Length: 45\r\n\r\n"
                        "I'm=going"
                        "&to=become"
                        "&the=king"
                        "&of=the"
                        "&pirates!!";

  Transaction t;
  // tmp
  confGroup   dummy_group;
  Config      dummy_conf = Config();
  dummy_group.push_back(&dummy_conf);

  t.parse_single_request(request, dummy_group);
  RequestInfo &info = t.get_request_info();

  EXPECT_EQ(info.values_[0], "I'm=going");
  EXPECT_EQ(info.values_[1], "to=become");
  EXPECT_EQ(info.values_[2], "the=king");
  EXPECT_EQ(info.values_[3], "of=the");
  EXPECT_EQ(info.values_[4], "pirates!!");
  // 今は"hoge=huga"の形でなくてもバリデートしてない
}

TEST(request_parse_test, query_body_capital) {
  std::string request = "POST /target HTTP/1.1\r\n"
                        "Host: 127.0.0.1:5001\r\n"
                        "Content-Type: AppliCation/x-WWW-form-URLENCODED\r\n"
                        "Content-Length: 38\r\n\r\n"
                        "yabu=kara"
                        "&stick="
                        "&ishi=no"
                        "&uenimo=3years";

  Transaction t;
  // tmp
  confGroup   dummy_group;
  Config      dummy_conf = Config();
  dummy_group.push_back(&dummy_conf);

  t.parse_single_request(request, dummy_group);
  RequestInfo &info = t.get_request_info();

  EXPECT_EQ(info.values_[0], "yabu=kara");
  EXPECT_EQ(info.values_[1], "stick=");
  EXPECT_EQ(info.values_[2], "ishi=no");
  EXPECT_EQ(info.values_[3], "uenimo=3years");
}
