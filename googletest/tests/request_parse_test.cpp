#include "gtest/gtest.h"

#include <string>

#include "event/Request.hpp"
#include "http/request/RequestInfo.hpp"

TEST(request_parse_test, normal) {
  std::string request_buffer = "GET / HTTP/1.1\r\n"
                               "Host: 127.0.0.1:50001\r\n"
                               "User-Agent: curl/7.68.0\r\n"
                               "Connection: close\r\n"
                               "Accept: */*\r\n\r\n";

  Config    config;
  confGroup conf_group;
  conf_group.push_back(&config);
  Request request;

  request.handle_request(request_buffer, conf_group);
  const RequestInfo &request_info = request.request_info();

  EXPECT_EQ(request_info.method_, "GET");
  EXPECT_EQ(request_info.request_target_, "/");
  EXPECT_EQ(request_info.version_, "HTTP/1.1");
  EXPECT_EQ(request_info.host_, "127.0.0.1");
  EXPECT_EQ(request_info.connection_close_, true);
}

TEST(request_parse_test, normal_delete) {
  std::string request_buffer = "DELETE /delete_target.tmp HTTP/1.1\r\n"
                               "Host: 127.0.0.1:50001\r\n"
                               "User-Agent: curl/7.68.0\r\n"
                               "Connection: close\r\n"
                               "Accept: */*\r\n\r\n";

  Config    config;
  confGroup conf_group;
  conf_group.push_back(&config);
  Request request;

  request.handle_request(request_buffer, conf_group);
  const RequestInfo &request_info = request.request_info();

  EXPECT_EQ(request_info.method_, "DELETE");
  EXPECT_EQ(request_info.request_target_, "/delete_target.tmp");
  EXPECT_EQ(request_info.version_, "HTTP/1.1");
  EXPECT_EQ(request_info.host_, "127.0.0.1");
  EXPECT_EQ(request_info.connection_close_, true);
}

TEST(request_parse_test, normal_post) {
  std::string request_buffer =
      "POST /target HTTP/1.1\r\n"
      "Host: 127.0.0.1:50001\r\n"
      "User-Agent: curl/7.68.0\r\n"
      "Connection: close\r\n"
      "Accept: */*\r\n"
      "Content-Type: applicATION/X-WWW-FORM-URlencoded\r\n"
      "Content-Length: 18\r\n\r\n";

  Config    config;
  confGroup conf_group;
  conf_group.push_back(&config);
  Request request;

  request.handle_request(request_buffer, conf_group);
  const RequestInfo &request_info = request.request_info();

  EXPECT_EQ(request_info.method_, "POST");
  EXPECT_EQ(request_info.request_target_, "/target");
  EXPECT_EQ(request_info.version_, "HTTP/1.1");
  EXPECT_EQ(request_info.host_, "127.0.0.1");
  EXPECT_EQ(request_info.content_type_.type_,
            "application/x-www-form-urlencoded");
  EXPECT_EQ(request_info.connection_close_, true);
  EXPECT_EQ(request_info.content_length_, static_cast<std::size_t>(18));
}

TEST(request_parse_test, query_string) {
  std::string request_buffer = "GET /hoge.cgi?name=taro&age=15 HTTP/1.1\r\n"
                               "Host: 127.0.0.1:50001\r\n"
                               "User-Agent: curl/7.68.0\r\n"
                               "Connection: close\r\n"
                               "Accept: */*\r\n\r\n";

  Config    config;
  confGroup conf_group;
  conf_group.push_back(&config);
  Request request;

  request.handle_request(request_buffer, conf_group);
  const RequestInfo &request_info = request.request_info();

  EXPECT_EQ(request_info.method_, "GET");
  EXPECT_EQ(request_info.request_target_, "/hoge.cgi");
  EXPECT_EQ(request_info.query_string_, "name=taro&age=15");
}

TEST(request_parse_test, select_location) {

  Config   config;
  Location location;
  location.location_path_ = "/";
  config.locations_.push_back(location);
  location.location_path_ = "/dir1/dir2/";
  config.locations_.push_back(location);
  location.location_path_ = "/dir1/";
  config.locations_.push_back(location);
  confGroup conf_group;
  conf_group.push_back(&config);

  {
    std::string request_buffer = "GET / HTTP/1.1\r\n"
                                 "Host: 127.0.0.1:50001\r\n"
                                 "\r\n";
    Request     request;
    request.handle_request(request_buffer, conf_group);
    const Location &request_location = request.request_info().location_;

    EXPECT_EQ(request_location.location_path_, "/");
  }
  {
    std::string request_buffer = "GET /dir1/ HTTP/1.1\r\n"
                                 "Host: 127.0.0.1:50001\r\n"
                                 "\r\n";
    Request     request;
    request.handle_request(request_buffer, conf_group);
    const Location &request_location = request.request_info().location_;

    EXPECT_EQ(request_location.location_path_, "/dir1/");
  }
  {
    std::string request_buffer = "GET /dir1/dir2/ HTTP/1.1\r\n"
                                 "Host: 127.0.0.1:50001\r\n"
                                 "\r\n";
    Request     request;
    request.handle_request(request_buffer, conf_group);
    const Location &request_location = request.request_info().location_;

    EXPECT_EQ(request_location.location_path_, "/dir1/dir2/");
  }
  {
    std::string request_buffer = "GET /nosush HTTP/1.1\r\n"
                                 "Host: 127.0.0.1:50001\r\n"
                                 "\r\n";
    Request     request;
    request.handle_request(request_buffer, conf_group);
    const Location &request_location = request.request_info().location_;

    EXPECT_EQ(request_location.location_path_, "/");
  }
}
