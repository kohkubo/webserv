#include "gtest/gtest.h"

#include <map>
#include <string>

#include "http/request/RequestInfo.hpp"

TEST(request_header_parse_test, exception_field_name_space) {
  std::string                        str = "Host : 127.0.0.1:5001";
  std::map<std::string, std::string> header_field_map;

  RequestInfo                        info;
  EXPECT_THROW(info.store_request_header_field_map(str, header_field_map),
               RequestInfo::BadRequestException);
}

TEST(request_header_parse_test, exception_field_name_tab) {
  std::string                        str = "Host\t: 127.0.0.1:5001";
  std::map<std::string, std::string> header_field_map;

  RequestInfo                        info;
  EXPECT_THROW(info.store_request_header_field_map(str, header_field_map),
               RequestInfo::BadRequestException);
}

TEST(request_header_parse_test, exception_no_host) {
  std::string str = "";
  std::map<std::string, std::string> header_field_map;

  RequestInfo info;
  EXPECT_THROW(info.store_request_header_field_map(str, header_field_map),
               RequestInfo::BadRequestException);
}

