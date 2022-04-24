#include <vector>
#include "ServerConfig.hpp"
#include "Webserv.hpp"
#include "gtest/gtest.h"

#define SAMPLE_CONF "../googletest/tdata/sample.conf"

TEST(read_config_test, read_server) {
  std::vector<ServerConfig> server_list = read_server_configuration(SAMPLE_CONF);

  EXPECT_EQ(server_list[0].listen_address_, "0.0.0.0");
  EXPECT_EQ(server_list[0].listen_port_, "5500");
  EXPECT_EQ(server_list[0].server_name_[0], "example.com");
  EXPECT_EQ(server_list[0].server_name_[1], "example.net");

  EXPECT_EQ(server_list[1].listen_address_, "0.0.0.0");
  EXPECT_EQ(server_list[1].listen_port_, "5500");
  EXPECT_EQ(server_list[1].root_, "/tmp/www");

  EXPECT_EQ(server_list[2].listen_address_, "0.0.0.0");
  EXPECT_EQ(server_list[2].listen_port_, "5001");
  EXPECT_EQ(server_list[0].server_name_[0], "example.com");
}
