#include "gtest/gtest.h"

#include <string>

#include "http/request/RequestInfo.hpp"

TEST(request_header_parse_test, exception_field_name_space) {
  std::string str = "Host : 127.0.0.1:5001";

  RequestInfo info;
  EXPECT_THROW(info.parse_request_header(str),
               RequestInfo::BadRequestException);
}

TEST(request_header_parse_test, exception_field_name_tab) {
  std::string str = "Host\t: 127.0.0.1:5001";

  RequestInfo info;
  EXPECT_THROW(info.parse_request_header(str),
               RequestInfo::BadRequestException);
}

TEST(request_header_parse_test, exception_no_host) {
  std::string str = "";

  RequestInfo info;
  EXPECT_THROW(info.parse_request_header(str),
               RequestInfo::BadRequestException);
}

TEST(request_header_parse_test, exception_host_range) {
  std::string str = "Host: 127.0.0.1:99999";
  std::string endline;

  RequestInfo info;
  info.parse_request_header(str);
  EXPECT_THROW(info.parse_request_header(endline),
               RequestInfo::BadRequestException);
}
