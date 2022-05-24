#include "gtest/gtest.h"

#include <string>

#include "http/request/RequestInfo.hpp"

TEST(request_header_parse_test, normal) {
  std::string str = "Host: 127.0.0.1:5001\r\n"
                    "User-Agent: curl/7.68.0\r\n"
                    "Connection: close\r\n"
                    "Accept: */*\r\n\r\n";

  RequestInfo info;
  info.parse_request_header(str);
  EXPECT_EQ(info.host_, "127.0.0.1");
  EXPECT_EQ(info.port_, "5001");
  EXPECT_EQ(info.is_close_, true);
  EXPECT_EQ(info.get_field_value("User-Agent"), "curl/7.68.0");
  EXPECT_EQ(info.get_field_value("Accept"), "*/*");
}

TEST(request_header_parse_test, exception_field_name_space) {
  std::string str = "Host : 127.0.0.1:5001\r\n\r\n";

  RequestInfo info;
  EXPECT_THROW(info.parse_request_header(str),
               RequestInfo::BadRequestException);
}

TEST(request_header_parse_test, exception_field_name_tab) {
  std::string str = "Host\t: 127.0.0.1:5001\r\n\r\n";

  RequestInfo info;
  EXPECT_THROW(info.parse_request_header(str),
               RequestInfo::BadRequestException);
}

TEST(request_header_parse_test, exception_no_host) {
  std::string str = "Connection: close\r\n"
                    "Accept: */*\r\n\r\n";

  RequestInfo info;
  EXPECT_THROW(info.parse_request_header(str),
               RequestInfo::BadRequestException);
}

TEST(request_header_parse_test, exception_host_range) {
  std::string str = "Host: 127.0.0.1:99999\r\n\r\n";

  RequestInfo info;
  EXPECT_THROW(info.parse_request_header(str),
               RequestInfo::BadRequestException);
}
