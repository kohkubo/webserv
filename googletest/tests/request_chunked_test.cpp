#include "gtest/gtest.h"

#include <string>

#include "config/Config.hpp"
#include "config/ConfigGroup.hpp"
#include "event/RequestHandler.hpp"
#include "http/request/RequestInfo.hpp"

TEST(request_chunked_test, chunked_body) {
  std::string request_buffer = "POST / HTTP/1.1\r\n"
                               "Host: hogehoge.com\r\n"
                               "Content-Type: text/plain\r\n"
                               "Transfer-Encoding: chunked\r\n"
                               "\r\n"
                               "8\r\n"
                               "Mozilla \r\n"
                               "28\r\n"
                               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\r\n"
                               "8\r\n"
                               " Network\r\n"
                               "10\r\n"
                               "hello world\ntest\r\n"
                               "0\r\n"
                               "\r\n";

  config::Config   config;
  config::Location location;
  location.root_          = "/";
  location.location_path_ = "/";
  config.locations_.add_or_exit(location);

  config::ConfigGroup config_group(config);
  RequestHandler      request_handler;

  request_handler.handle_request(request_buffer, config_group);
  const RequestInfo &request_info = request_handler.request_info();

  EXPECT_EQ(request_info.is_chunked(), true);
  EXPECT_EQ(request_info.content_info_.content_,
            "Mozilla aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa "
            "Networkhello world\ntest");
}

TEST(request_chunked_test, chunked_body_split) {
  std::string request_buffer1 = "POST / HTTP/1.1\r\n"
                                "Host: hogehoge.com\r\n"
                                "Content-Type: text/plain\r\n"
                                "Transfer-Encoding: chunked\r\n"
                                "\r\n"
                                "8\r\n"
                                "Moz";
  std::string request_buffer2 = "illa \r\n"
                                "2";
  std::string request_buffer3 = "8\r\n"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\r\n"
                                "8\r\n"
                                " Network\r\n"
                                "10\r\n"
                                "hello world\ntest\r\n"
                                "0\r\n"
                                "\r\n";

  config::Config   config;
  config::Location location;
  location.root_          = "/";
  location.location_path_ = "/";
  config.locations_.add_or_exit(location);
  config::ConfigGroup config_group(config);
  RequestHandler      request_handler;
  std::string         request_buffer;
  request_buffer.append(request_buffer1);
  request_handler.handle_request(request_buffer, config_group);
  request_buffer.append(request_buffer2);
  request_handler.handle_request(request_buffer, config_group);
  request_buffer.append(request_buffer3);
  request_handler.handle_request(request_buffer, config_group);

  const RequestInfo &request_info = request_handler.request_info();

  EXPECT_EQ(request_info.is_chunked(), true);
  EXPECT_EQ(request_info.content_info_.content_,
            "Mozilla aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa Networkhello "
            "world\ntest");
}
