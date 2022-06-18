#include "gtest/gtest.h"

#include <string>

#include "http/request/RequestInfo.hpp"

TEST(request_startline_parse_test, normal) {
  std::string str = "GET / HTTP/1.1";

  RequestInfo request_info;
  request_info.parse_request_start_line(str);
  EXPECT_EQ(request_info.method_, "GET");
  EXPECT_EQ(request_info.request_target_, "/");
  EXPECT_EQ(request_info.version_, "HTTP/1.1");
}

TEST(request_startline_parse_test, normal_delete) {
  std::string str = "DELETE /delete_target.tmp HTTP/1.1";

  RequestInfo request_info;
  request_info.parse_request_start_line(str);
  EXPECT_EQ(request_info.method_, "DELETE");
  EXPECT_EQ(request_info.request_target_, "/delete_target.tmp");
  EXPECT_EQ(request_info.version_, "HTTP/1.1");
}

TEST(request_startline_parse_test, exception_request_line_few_field) {
  std::string str = "GET HTTP/1.1";

  RequestInfo request_info;
  EXPECT_THROW(request_info.check_bad_parse_request_start_line(str),
               RequestInfo::BadRequestException);
}

TEST(request_startline_parse_test, exception_request_line_no_space) {
  std::string str = "GET/HTTP/1.1";

  RequestInfo request_info;
  EXPECT_THROW(request_info.check_bad_parse_request_start_line(str),
               RequestInfo::BadRequestException);
}
