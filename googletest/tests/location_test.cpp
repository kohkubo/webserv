#include "gtest/gtest.h"

#include "config/Config.hpp"
#include "config/Location.hpp"
#include "event/Server.hpp"
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

static Location select_proper_location(const std::string           &request_uri,
                                       const std::vector<Location> &locations) {
  // clang-format off
  std::string path;
  // clang-format on
  std::vector<Location>::const_iterator it = locations.begin();
  for (; it != locations.end(); ++it) {
    if (request_uri.find(it->location_path_) == 0) {
      if (path.size() < it->location_path_.size()) {
        path = it->location_path_;
        return *it;
      }
    }
  }
  LOG("location is null");
  throw RequestInfo::BadRequestException(HttpStatusCode::NOT_FOUND_404);
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
  request_info.config_         = *config;
  request_info.request_target_ = request_target;
  request_info.location_ =
      select_proper_location(request_target, config->locations_);
  request_info.method_ = "GET";
  request_info.file_path_ =
      create_file_path(request_target, request_info.location_);

  std::string body = get_http_response_body(
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
  request_info.config_ = *config;
  request_info.location_ =
      select_proper_location(request_target, config->locations_);
  request_info.request_target_ = request_target;
  request_info.method_         = "GET";
  request_info.file_path_ =
      create_file_path(request_target, request_info.location_);
  int status_code = get_http_response_status_code(
      ResponseGenerator::generate_response(request_info));
  bool res = status_code == expected_status_code;
  if (!res) {
    std::cout << "expected: " << expected_status_code << std::endl;
    std::cout << "actual: " << status_code << std::endl;
  }
  return res;
}
