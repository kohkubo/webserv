#include "gtest/gtest.h"

#include <string>

#include "http/request/RequestInfo.hpp"

TEST(request_body_parse_test, simple) {
  std::string              request_body = "I'm=going"
                                          "&to=become"
                                          "&the=king"
                                          "&of=the"
                                          "&pirates!!";
  RequestInfo::ContentInfo content_type;
  content_type.type_ = "application/x-www-form-urlencoded";
  RequestInfo info;
  info.parse_request_body(request_body, content_type);
  EXPECT_EQ(info.env_values_[0], "I'm=going");
  EXPECT_EQ(info.env_values_[1], "to=become");
  EXPECT_EQ(info.env_values_[2], "the=king");
  EXPECT_EQ(info.env_values_[3], "of=the");
  EXPECT_EQ(info.env_values_[4], "pirates!!");
  // 今は"hoge=huga"の形でなくてもバリデートしてない
}

TEST(request_body_parse_test, simple) {
  std::string              request_body = "I'm=going"
                                          "&to=become"
                                          "&the=king"
                                          "&of=the"
                                          "&pirates!!";
  RequestInfo::ContentInfo content_type;
  content_type.type_ = "application/x-www-form-urlencoded";
  RequestInfo info;
  info.parse_request_body(request_body, content_type);
  EXPECT_EQ(info.env_values_[0], "I'm=going");
  EXPECT_EQ(info.env_values_[1], "to=become");
  EXPECT_EQ(info.env_values_[2], "the=king");
  EXPECT_EQ(info.env_values_[3], "of=the");
  EXPECT_EQ(info.env_values_[4], "pirates!!");
  // 今は"hoge=huga"の形でなくてもバリデートしてない
}
