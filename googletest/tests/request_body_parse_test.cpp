#include "gtest/gtest.h"

#include <string>

#include "http/request/RequestInfo.hpp"

TEST(request_body_parse_test, urlencoded) {
  std::string              request_body = "I'm=going"
                                          "&to=become"
                                          "&the=king"
                                          "&of=the"
                                          "&pirates!!";
  RequestInfo::ContentInfo content_info;
  content_info.type_ = "application/x-www-form-urlencoded";
  RequestInfo request_info;
  request_info.parse_request_body(request_body, content_info);
  EXPECT_EQ(request_info.body_, request_body);
}
