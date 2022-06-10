#include "gtest/gtest.h"

#include <map>
#include <string>

#include "http/request/RequestInfo.hpp"

TEST(request_header_contenttype_parse_test, parameter) {
  std::map<std::string, std::string> header_field_map;
  header_field_map["Host"]         = "127.0.0.1:5001";
  header_field_map["Content-Type"] = "text/html  ;     charset=utf-8";

  RequestInfo info;
  info.parse_request_header(header_field_map);
  EXPECT_EQ(info.content_type_.first_, "text/html");
  EXPECT_EQ(info.content_type_.parameter_["charset"], "utf-8");
}

TEST(request_header_contenttype_parse_test, quoted_parameter) {
  std::map<std::string, std::string> header_field_map;
  header_field_map["Host"] = "127.0.0.1:5001";
  header_field_map["Content-Type"] =
      "mulTIPARt/form-data;boUNDAry=\"boundary\"";

  RequestInfo info;
  info.parse_request_header(header_field_map);
  EXPECT_EQ(info.content_type_.first_, "multipart/form-data");
  EXPECT_EQ(info.content_type_.parameter_["boundary"], "boundary");
}

TEST(request_header_contenttype_parse_test, quoted_parameter_onechar) {
  std::map<std::string, std::string> header_field_map;
  header_field_map["Host"]         = "127.0.0.1:5001";
  header_field_map["Content-Type"] = "multipart/form-data;boundary=\"_\"";

  RequestInfo info;
  info.parse_request_header(header_field_map);
  EXPECT_EQ(info.content_type_.first_, "multipart/form-data");
  EXPECT_EQ(info.content_type_.parameter_["boundary"], "_");
}

TEST(request_header_contenttype_parse_test, parameter_empty_value) {
  std::map<std::string, std::string> header_field_map;
  header_field_map["Host"]         = "127.0.0.1:5001";
  header_field_map["Content-Type"] = "multipart/form-data;boundary=";

  RequestInfo info;
  info.parse_request_header(header_field_map);
  EXPECT_EQ(info.content_type_.first_, "multipart/form-data");
  EXPECT_EQ(info.content_type_.parameter_["boundary"], "");
}

TEST(request_header_contenttype_parse_test, parameter_empty_key) {
  std::map<std::string, std::string> header_field_map;
  header_field_map["Host"]         = "127.0.0.1:5001";
  header_field_map["Content-Type"] = "multipart/form-data;=value";

  RequestInfo info;
  info.parse_request_header(header_field_map);
  EXPECT_EQ(info.content_type_.first_, "multipart/form-data");
}

TEST(request_header_contenttype_parse_test, quoted_parameter_empty_value) {
  std::map<std::string, std::string> header_field_map;
  header_field_map["Host"]         = "127.0.0.1:5001";
  header_field_map["Content-Type"] = "multipart/form-data;boundary=\"\"";

  RequestInfo info;
  info.parse_request_header(header_field_map);
  EXPECT_EQ(info.content_type_.first_, "multipart/form-data");
  EXPECT_EQ(info.content_type_.parameter_["boundary"], "");
}

TEST(request_header_contenttype_parse_test,
     exception_contenttype_missingequal) {
  std::map<std::string, std::string> header_field_map;
  header_field_map["Host"]         = "127.0.0.1:5001";
  header_field_map["Content-Type"] = "multipart/form-data;boundary\"boundary\"";

  RequestInfo info;

  EXPECT_THROW(info.parse_request_header(header_field_map),
               RequestInfo::BadRequestException);
}

TEST(request_header_contenttype_parse_test, exception_contenttype_onequote) {
  std::map<std::string, std::string> header_field_map;
  header_field_map["Host"]         = "127.0.0.1:5001";
  header_field_map["Content-Type"] = "multipart/form-data;boundary=\"";

  RequestInfo info;

  EXPECT_THROW(info.parse_request_header(header_field_map),
               RequestInfo::BadRequestException);
}

TEST(request_header_contenttype_parse_test,
     exception_contenttype_lackingquote_head) {
  std::map<std::string, std::string> header_field_map;
  header_field_map["Host"]         = "127.0.0.1:5001";
  header_field_map["Content-Type"] = "multipart/form-data;boundary=\"aaa";

  RequestInfo info;

  EXPECT_THROW(info.parse_request_header(header_field_map),
               RequestInfo::BadRequestException);
}

TEST(request_header_contenttype_parse_test,
     exception_contenttype_lackingquote_end) {
  std::map<std::string, std::string> header_field_map;
  header_field_map["Host"]         = "127.0.0.1:5001";
  header_field_map["Content-Type"] = "multipart/form-data;boundary=aaa\"";

  RequestInfo info;

  EXPECT_THROW(info.parse_request_header(header_field_map),
               RequestInfo::BadRequestException);
}
