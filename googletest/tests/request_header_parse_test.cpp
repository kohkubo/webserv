#include "gtest/gtest.h"

#include <map>
#include <string>

#include "http/request/RequestInfo.hpp"

TEST(request_header_parse_test, exception_field_name_space) {
  std::string str = "Host : 127.0.0.1:50001";

  RequestInfo request_info;
  EXPECT_FALSE(request_info.header_field_map_.store_new_field(str));
}

TEST(request_header_parse_test, exception_field_name_tab) {
  std::string str = "Host\t: 127.0.0.1:50001";

  RequestInfo request_info;
  EXPECT_FALSE(request_info.header_field_map_.store_new_field(str));
}

TEST(request_header_parse_test, exception_no_host) {
  std::string str = "";

  RequestInfo request_info;
  EXPECT_FALSE(request_info.header_field_map_.store_new_field(str));
}
