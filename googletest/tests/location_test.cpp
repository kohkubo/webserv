#include "gtest/gtest.h"

#include "config/ConfGroupMapGenerator.hpp"
#include "config/Config.hpp"
#include "config/Location.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/Response.hpp"
#include "utils/tokenize.hpp"

#define SAMPLE_CONF "tdata/location.conf"

std::string get_http_response_body(const std::string &response_string) {
  std::string body =
      response_string.substr(response_string.find("\r\n\r\n") + 4);
  return body;
}

bool test_request_body(const Config *config, const std::string &uri,
                       const std::string &expected_body) {
  RequestInfo req_info;
  req_info.uri_    = uri;
  req_info.method_ = GET;
  Response response(*config, req_info);
  std::string body = get_http_response_body(response.get_response_string());
  bool res = body == expected_body;
  if (!res) {
    std::cout << "expected: " << expected_body << std::endl;
    std::cout << "actual: " << body << std::endl;
  }
  return res;
}

TEST(location_test, multi_location) {
  {
    ConfGroupMapGenerator         config_map_generator(SAMPLE_CONF);
    std::map<listenFd, confGroup> conf_group_map =
        config_map_generator.generate();
    EXPECT_EQ(5, conf_group_map.size());
    const Config *config = conf_group_map.begin()->second[0];
    EXPECT_EQ(4, config->locations_.size());
    EXPECT_EQ("/", config->locations_[0].location_path_);
    EXPECT_EQ("/dir/", config->locations_[1].location_path_);
    EXPECT_EQ("/test_dir/dir/", config->locations_[2].location_path_);
    EXPECT_EQ("/test_dir/", config->locations_[3].location_path_);

    EXPECT_TRUE(test_request_body(config, "/", "location!\n"));
    EXPECT_TRUE(test_request_body(config, "/dir/", "location!\n"));
    EXPECT_TRUE(test_request_body(config, "/test_dir/", "location! test_dir\n"));
    EXPECT_TRUE(test_request_body(config, "/dir/index.html", "location! dir\n"));
  }
}