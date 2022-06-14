#include "gtest/gtest.h"

#include <string>

#include "event/ReceiveRequest.hpp"
#include "http/request/RequestInfo.hpp"

TEST(request_parse_test, normal) {
  std::string request = "GET / HTTP/1.1\r\n"
                        "Host: 127.0.0.1:50001\r\n"
                        "User-Agent: curl/7.68.0\r\n"
                        "Connection: close\r\n"
                        "Accept: */*\r\n\r\n";

  Config      config;
  confGroup   conf_group;
  conf_group.push_back(&config);
  ReceiveRequest transaction;

  transaction.handle_request(request, conf_group);
  const RequestInfo &r = transaction.request_info();

  EXPECT_EQ(r.method_, "GET");
  EXPECT_EQ(r.uri_, "/");
  EXPECT_EQ(r.version_, "HTTP/1.1");
  EXPECT_EQ(r.host_, "127.0.0.1");
  EXPECT_EQ(r.connection_close_, true);
}

TEST(request_parse_test, normal_delete) {
  std::string request = "DELETE /delete_target.tmp HTTP/1.1\r\n"
                        "Host: 127.0.0.1:50001\r\n"
                        "User-Agent: curl/7.68.0\r\n"
                        "Connection: close\r\n"
                        "Accept: */*\r\n\r\n";

  Config      config;
  confGroup   conf_group;
  conf_group.push_back(&config);
  ReceiveRequest transaction;

  transaction.handle_request(request, conf_group);
  const RequestInfo &r = transaction.request_info();

  EXPECT_EQ(r.method_, "DELETE");
  EXPECT_EQ(r.uri_, "/delete_target.tmp");
  EXPECT_EQ(r.version_, "HTTP/1.1");
  EXPECT_EQ(r.host_, "127.0.0.1");
  EXPECT_EQ(r.connection_close_, true);
}

TEST(request_parse_test, normal_post) {
  std::string request = "POST /target HTTP/1.1\r\n"
                        "Host: 127.0.0.1:50001\r\n"
                        "User-Agent: curl/7.68.0\r\n"
                        "Connection: close\r\n"
                        "Accept: */*\r\n"
                        "Content-Type: applicATION/X-WWW-FORM-URlencoded\r\n"
                        "Content-Length: 18\r\n\r\n";

  Config      config;
  confGroup   conf_group;
  conf_group.push_back(&config);
  ReceiveRequest transaction;

  transaction.handle_request(request, conf_group);
  const RequestInfo &r = transaction.request_info();

  EXPECT_EQ(r.method_, "POST");
  EXPECT_EQ(r.uri_, "/target");
  EXPECT_EQ(r.version_, "HTTP/1.1");
  EXPECT_EQ(r.host_, "127.0.0.1");
  EXPECT_EQ(r.content_type_.type_, "application/x-www-form-urlencoded");
  EXPECT_EQ(r.connection_close_, true);
  EXPECT_EQ(r.content_length_, static_cast<std::size_t>(18));
}

TEST(request_parse_test, query_body) {
  std::string request = "POST /target HTTP/1.1\r\n"
                        "Host: 127.0.0.1:50001\r\n"
                        "Content-Type: AppliCation/x-WWW-form-URLENCODED\r\n"
                        "Content-Length: 45\r\n\r\n"
                        "I'm=going"
                        "&to=become"
                        "&the=king"
                        "&of=the"
                        "&pirates!!";

  Config      config;
  confGroup   conf_group;
  conf_group.push_back(&config);
  ReceiveRequest transaction;

  transaction.handle_request(request, conf_group);
  const RequestInfo &info = transaction.request_info();
  EXPECT_EQ(info.env_values_[0], "I'm=going");
  EXPECT_EQ(info.env_values_[1], "to=become");
  EXPECT_EQ(info.env_values_[2], "the=king");
  EXPECT_EQ(info.env_values_[3], "of=the");
  EXPECT_EQ(info.env_values_[4], "pirates!!");
  // 今は"hoge=huga"の形でなくてもバリデートしてない
}
