#include "gtest/gtest.h"

#include <string>

#include "config/ConfigGroup.hpp"
#include "http/request/RequestHandler.hpp"
#include "http/request/RequestInfo.hpp"

TEST(request_parse_test, normal) {
  std::string request_buffer = "GET / HTTP/1.1\r\n"
                               "Host: 127.0.0.1:50001\r\n"
                               "User-Agent: curl/7.68.0\r\n"
                               "Connection: close\r\n"
                               "Accept: */*\r\n\r\n";

  config::Config      config;
  config::ConfigGroup config_group(config);
  RequestHandler      request_handler;

  request_handler.handle_request(request_buffer, config_group);
  const RequestInfo &request_info = request_handler.request_info();

  EXPECT_EQ(request_info.request_line_.method_, "GET");
  EXPECT_EQ(request_info.request_line_.absolute_path_.str(), "/");
  EXPECT_EQ(request_info.request_line_.http_version_, "HTTP/1.1");
  EXPECT_EQ(request_info.host_, "127.0.0.1");
  EXPECT_EQ(request_info.is_close_connection(), true);
}

TEST(request_parse_test, normal_delete) {
  std::string request_buffer = "DELETE /delete_target.tmp HTTP/1.1\r\n"
                               "Host: 127.0.0.1:50001\r\n"
                               "User-Agent: curl/7.68.0\r\n"
                               "Connection: close\r\n"
                               "Accept: */*\r\n\r\n";

  config::Config      config;
  config::ConfigGroup config_group(config);
  RequestHandler      request_handler;

  request_handler.handle_request(request_buffer, config_group);
  const RequestInfo &request_info = request_handler.request_info();

  EXPECT_EQ(request_info.request_line_.method_, "DELETE");
  EXPECT_EQ(request_info.request_line_.absolute_path_.str(),
            "/delete_target.tmp");
  EXPECT_EQ(request_info.request_line_.http_version_, "HTTP/1.1");
  EXPECT_EQ(request_info.host_, "127.0.0.1");
  EXPECT_EQ(request_info.is_close_connection(), true);
}

TEST(request_parse_test, normal_post) {
  std::string request_buffer =
      "POST /target HTTP/1.1\r\n"
      "Host: 127.0.0.1:50001\r\n"
      "User-Agent: curl/7.68.0\r\n"
      "Connection: close\r\n"
      "Accept: */*\r\n"
      "Content-Type: application/x-www-form-urlencoded\r\n"
      "Content-Length: 18\r\n\r\n";

  config::Config      config;
  config::ConfigGroup config_group(config);
  RequestHandler      request_handler;

  request_handler.handle_request(request_buffer, config_group);
  const RequestInfo &request_info = request_handler.request_info();

  EXPECT_EQ(request_info.request_line_.method_, "POST");
  EXPECT_EQ(request_info.request_line_.absolute_path_.str(), "/target");
  EXPECT_EQ(request_info.request_line_.http_version_, "HTTP/1.1");
  EXPECT_EQ(request_info.host_, "127.0.0.1");
  EXPECT_EQ(request_info.content_info_.content_type_,
            "application/x-www-form-urlencoded");
  EXPECT_EQ(request_info.is_close_connection(), true);
  EXPECT_EQ(request_info.content_info_.content_length_,
            static_cast<std::size_t>(18));
}

TEST(request_parse_test, query_string) {
  std::string request_buffer = "GET /hoge.cgi?name=taro&age=15 HTTP/1.1\r\n"
                               "Host: 127.0.0.1:50001\r\n"
                               "User-Agent: curl/7.68.0\r\n"
                               "Connection: close\r\n"
                               "Accept: */*\r\n\r\n";

  config::Config      config;
  config::ConfigGroup config_group(config);
  RequestHandler      request_handler;

  request_handler.handle_request(request_buffer, config_group);
  const RequestInfo &request_info = request_handler.request_info();

  EXPECT_EQ(request_info.request_line_.method_, "GET");
  EXPECT_EQ(request_info.request_line_.absolute_path_.str(), "/hoge.cgi");
  EXPECT_EQ(request_info.request_line_.query_, "name=taro&age=15");
}
