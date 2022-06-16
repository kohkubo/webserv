#include "gtest/gtest.h"

#include <string>

#include "http/request/RequestInfo.hpp"

TEST(request_body_parse_test, urlencoded) {
  std::string              request_body = "I'm=going"
                                          "&to=become"
                                          "&the=king"
                                          "&of=the"
                                          "&pirates!!";
  RequestInfo::ContentInfo content_type;
  content_type.type_ = "application/x-www-form-urlencoded";
  RequestInfo info;
  info.parse_request_body(request_body, content_type);
  EXPECT_EQ(info.body_, request_body);
}

TEST(request_body_parse_test, multi_part) {
  // clang-format off
  std::string request_body = "------WebKitFormBoundaryhBP36BYMHJOCgZsX\r\n"
                             "Content-Disposition: form-data; name=\"name\"\r\n"
                             "\r\n"
                             "aaa\r\n"
                             "------WebKitFormBoundaryhBP36BYMHJOCgZsX\r\n"
                             "Content-Disposition: form-data; name=\"upfile\"; filename=\"test.txt\"\r\n"
                             "Content-Type: text/plain\r\n"
                             "\r\n"
                             "content\r\n"
                             "------WebKitFormBoundaryhBP36BYMHJOCgZsX--\r\n";
  RequestInfo::ContentInfo content_type;
  content_type.type_ = "multipart/form-data";
  content_type.parameter_["boundary"] = "----WebKitFormBoundaryhBP36BYMHJOCgZsX";
  RequestInfo info;
  info.parse_request_body(request_body, content_type);
  RequestInfo::Form first_form  = info.form_map_["name"];
  RequestInfo::Form second_form = info.form_map_["upfile"];
  EXPECT_EQ(first_form.content_disposition_.type_, "form-data");
  EXPECT_EQ(first_form.content_disposition_.parameter_["name"], "name");
  EXPECT_EQ(first_form.content_, "aaa");
  EXPECT_EQ(second_form.content_disposition_.type_, "form-data");
  EXPECT_EQ(second_form.content_disposition_.parameter_["name"], "upfile");
  EXPECT_EQ(second_form.content_disposition_.parameter_["filename"], "test.txt");
  EXPECT_EQ(second_form.content_type_.type_, "text/plain");
  EXPECT_EQ(second_form.content_, "content");
  // clang-format on
}

TEST(request_body_parse_test, multi_part_content_has_CRLF) {
  // clang-format off
  std::string request_body = "------WebKitFormBoundaryhBP36BYMHJOCgZsX\r\n"
                             "Content-Disposition: form-data; name=\"name\"\r\n"
                             "\r\n"
                             "aaa\r\n"
                             "------WebKitFormBoundaryhBP36BYMHJOCgZsX\r\n"
                             "Content-Disposition: form-data; name=\"upfile\"; filename=\"test.txt\"\r\n"
                             "Content-Type: text/plain\r\n"
                             "\r\n"
                             "con\r\nte\r\nnt\r\n"
                             "------WebKitFormBoundaryhBP36BYMHJOCgZsX--\r\n";
  RequestInfo::ContentInfo content_type;
  content_type.type_ = "multipart/form-data";
  content_type.parameter_["boundary"] = "----WebKitFormBoundaryhBP36BYMHJOCgZsX";
  RequestInfo info;
  info.parse_request_body(request_body, content_type);
  RequestInfo::Form first_form  = info.form_map_["name"];
  RequestInfo::Form second_form = info.form_map_["upfile"];
  EXPECT_EQ(first_form.content_disposition_.type_, "form-data");
  EXPECT_EQ(first_form.content_disposition_.parameter_["name"], "name");
  EXPECT_EQ(first_form.content_, "aaa");
  EXPECT_EQ(second_form.content_disposition_.type_, "form-data");
  EXPECT_EQ(second_form.content_disposition_.parameter_["name"], "upfile");
  EXPECT_EQ(second_form.content_disposition_.parameter_["filename"], "test.txt");
  EXPECT_EQ(second_form.content_type_.type_, "text/plain");
  EXPECT_EQ(second_form.content_, "con\r\nte\r\nnt");
  // clang-format on
}

TEST(request_body_parse_test, exception_multipart_missing_contentdisposition) {
  // clang-format off
  std::string request_body = "------WebKitFormBoundaryhBP36BYMHJOCgZsX\r\n"
                             "Content-Type: text/plain\r\n"
                             "\r\n"
                             "content\r\n"
                             "------WebKitFormBoundaryhBP36BYMHJOCgZsX--\r\n";
  RequestInfo::ContentInfo content_type;
  content_type.type_ = "multipart/form-data";
  content_type.parameter_["boundary"] = "----WebKitFormBoundaryhBP36BYMHJOCgZsX";
  RequestInfo info;
  EXPECT_THROW(info.parse_request_body(request_body, content_type),
               RequestInfo::BadRequestException);
  // clang-format on
}

TEST(request_body_parse_test, exception_multipart_nosuchtype) {
  // clang-format off
  std::string request_body = "------WebKitFormBoundaryhBP36BYMHJOCgZsX\r\n"
                             "Content-Disposition: nosuch; name=\"upfile\"; filename=\"test.txt\"\r\n"
                             "Content-Type: text/plain\r\n"
                             "\r\n"
                             "content\r\n"
                             "------WebKitFormBoundaryhBP36BYMHJOCgZsX--\r\n";
  RequestInfo::ContentInfo content_type;
  content_type.type_ = "multipart/form-data";
  content_type.parameter_["boundary"] = "----WebKitFormBoundaryhBP36BYMHJOCgZsX";
  RequestInfo info;
  EXPECT_THROW(info.parse_request_body(request_body, content_type),
               RequestInfo::BadRequestException);
  // clang-format on
}

TEST(request_body_parse_test, exception_multipart_missing_name) {
  // clang-format off
   std::string request_body = "------WebKitFormBoundaryhBP36BYMHJOCgZsX\r\n"
                              "Content-Disposition: form-data; filename=\"test.txt\"\r\n"
                              "Content-Type: text/plain\r\n"
                              "\r\n"
                              "content\r\n"
                              "------WebKitFormBoundaryhBP36BYMHJOCgZsX--\r\n";
   RequestInfo::ContentInfo content_type;
   content_type.type_ = "multipart/form-data";
   content_type.parameter_["boundary"] = "----WebKitFormBoundaryhBP36BYMHJOCgZsX";
   RequestInfo info;
   EXPECT_THROW(info.parse_request_body(request_body, content_type),
                RequestInfo::BadRequestException);
  // clang-format on
}
