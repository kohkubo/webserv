#include "gtest/gtest.h"

#include <string>

#include "config/Config.hpp"
#include "config/ConfigGroup.hpp"
#include "event/Request.hpp"
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
  Request             request;

  request.handle_request(request_buffer, config_group);
  const RequestInfo &info = request.request_info();

  EXPECT_EQ(info.is_chunked(), true);
  EXPECT_EQ(info.body_, "Mozilla aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa "
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
  Request             request;
  std::string         request_buffer;
  request_buffer.append(request_buffer1);
  request.handle_request(request_buffer, config_group);
  request_buffer.append(request_buffer2);
  request.handle_request(request_buffer, config_group);
  request_buffer.append(request_buffer3);
  request.handle_request(request_buffer, config_group);

  const RequestInfo &info = request.request_info();

  EXPECT_EQ(info.is_chunked(), true);
  EXPECT_EQ(info.body_,
            "Mozilla aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa Networkhello "
            "world\ntest");
}
