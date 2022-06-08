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
  std::string                        str = "";
  std::map<std::string, std::string> header_field_map;

  RequestInfo                        info;
  EXPECT_THROW(info.store_request_header_field_map(str, header_field_map),
               RequestInfo::BadRequestException);
}

TEST(request_header_parse_test, contenttype_parameter) {
  std::map<std::string, std::string> header_field_map;
  header_field_map["Host"] = "127.0.0.1:5001";
  header_field_map["Content-Type"] =
      "mulTIPARt/form-data;boUNDAry=\"boundary\"";

  RequestInfo info;
  info.parse_request_header(header_field_map);
  EXPECT_EQ(info.content_type_, "multipart/form-data");
  EXPECT_EQ(info.parameter_["boundary"], "boundary");
}

TEST(request_parse_test, exception_contenttype_missingequal) {
  std::map<std::string, std::string> header_field_map;
  header_field_map["Host"]         = "127.0.0.1:5001";
  header_field_map["Content-Type"] = "multipart/form-data;boundary\"boundary\"";

  RequestInfo info;

  EXPECT_THROW(info.parse_request_header(header_field_map),
               RequestInfo::BadRequestException);
}

TEST(request_parse_test, exception_contenttype_missingquote) {
  std::map<std::string, std::string> header_field_map;
  header_field_map["Host"]         = "127.0.0.1:5001";
  header_field_map["Content-Type"] = "multipart/form-data;boundary=\"";

  RequestInfo info;

  EXPECT_THROW(info.parse_request_header(header_field_map),
               RequestInfo::BadRequestException);
}
