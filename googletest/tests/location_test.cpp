#include "gtest/gtest.h"

#include "config/ConfGroupMapGenerator.hpp"
#include "config/Config.hpp"
#include "config/Location.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

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

// 最長マッチ
static const Location *
select_proper_location(const std::string           &request_uri,
                       const std::vector<Location> &locations) {
  // clang-format off
  std::string     path;
  const Location *location = NULL;
  // clang-format on
  std::vector<Location>::const_iterator it = locations.begin();
  for (; it != locations.end(); ++it) {
    if (request_uri.find(it->location_path_) == 0) {
      if (path.size() < it->location_path_.size()) {
        path     = it->location_path_;
        location = &(*it);
      }
    }
  }
  return location;
}

static std::string create_file_path(const std::string &request_target,
                                    const Location    &location) {
  std::string file_path = location.root_ + request_target;
  if (has_suffix(file_path, "/") &&
      is_file_exists(file_path + location.index_)) {
    file_path += location.index_;
  }
  return file_path;
}

bool test_request_body(const Config *config, const std::string &request_target,
                       const std::string &expected_body) {
  RequestInfo request_info;
  request_info.config_         = config;
  request_info.request_target_ = request_target;
  request_info.location_ =
      select_proper_location(request_target, config->locations_);
  request_info.method_    = "GET";
  request_info.file_path_ = create_file_path(request_target,
                                              *request_info.location_);

  std::string body        = get_http_response_body(
             ResponseGenerator::generate_response(request_info));
  bool res = body == expected_body;
  if (!res) {
    std::cout << "expected: " << expected_body << std::endl;
    std::cout << "actual: " << body << std::endl;
  }
  return res;
}

bool test_request_status_code(const Config      *config,
                              const std::string &request_target,
                              int                expected_status_code) {
  RequestInfo request_info;
  request_info.config_ = config;
  request_info.location_ =
      select_proper_location(request_target, config->locations_);
  request_info.request_target_ = request_target;
  request_info.method_         = "GET";
  request_info.file_path_      = create_file_path(request_target,
                                                  *request_info.location_);
  int status_code              = get_http_response_status_code(
                   ResponseGenerator::generate_response(request_info));
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
    EXPECT_EQ(static_cast<std::size_t>(5), conf_group_map.size());
    const Config *config = conf_group_map.begin()->second[0];
    EXPECT_EQ(static_cast<std::size_t>(5), config->locations_.size());
    EXPECT_EQ("/", config->locations_[0].location_path_);
    EXPECT_EQ("/dir/", config->locations_[1].location_path_);
    EXPECT_EQ("/test_dir/dir/", config->locations_[2].location_path_);
    EXPECT_EQ("/test_dir/", config->locations_[3].location_path_);
    EXPECT_EQ("/dir2/", config->locations_[4].location_path_);

    EXPECT_TRUE(test_request_status_code(config, "/dir2/", 404));
    EXPECT_TRUE(test_request_body(config, "/", "location!\n"));
    EXPECT_TRUE(test_request_body(config, "/dir/", "location! dir\n"));
    EXPECT_TRUE(
        test_request_body(config, "/test_dir/", "location! test_dir\n"));
    EXPECT_TRUE(
        test_request_body(config, "/dir/index.html", "location! dir\n"));
    // close all sockets
    std::map<listenFd, confGroup>::iterator it = conf_group_map.begin();
    for (; it != conf_group_map.end(); it++) {
      close(it->first);
    }
  }
}
