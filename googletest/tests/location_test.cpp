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

int get_http_response_status_code(const std::string &response_string) {
  std::string code_str =
      response_string.substr(response_string.find("<title>") + 7, 3);
  return std::stoi(code_str);
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

bool test_request_status_code(const Config *config, const std::string &uri,
                              int expected_status_code) {
  RequestInfo req_info;
  req_info.uri_    = uri;
  req_info.method_ = GET;
  Response response(*config, req_info);
  int status_code = get_http_response_status_code(response.get_response_string());
  bool res = status_code == expected_status_code;
  if (!res) {
    std::cout << "expected: " << expected_status_code << std::endl;
    std::cout << "actual: " << status_code << std::endl;
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
    EXPECT_EQ(5, config->locations_.size());
    EXPECT_EQ("/", config->locations_[0].location_path_);
    EXPECT_EQ("/dir/", config->locations_[1].location_path_);
    EXPECT_EQ("/test_dir/dir/", config->locations_[2].location_path_);
    EXPECT_EQ("/test_dir/", config->locations_[3].location_path_);
    EXPECT_EQ("/dir2/", config->locations_[4].location_path_);

    EXPECT_TRUE(test_request_status_code(config, "/dir2/", 404));
    EXPECT_TRUE(test_request_body(config, "/", "location!\n"));
    EXPECT_TRUE(test_request_body(config, "/dir/", "location! dir\n"));
    EXPECT_TRUE(test_request_body(config, "/test_dir/", "location! test_dir\n"));
    EXPECT_TRUE(test_request_body(config, "/dir/index.html", "location! dir\n"));
  }
}
