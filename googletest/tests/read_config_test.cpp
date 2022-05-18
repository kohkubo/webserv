#include "config/ServerConfig.hpp"
#include "config/config.hpp"
#include "gtest/gtest.h"
#include <vector>

#define SAMPLE_CONF "../googletest/tdata/sample.conf"

TEST(read_config_test, simple_test) {
  server_list server_list = read_config(SAMPLE_CONF);

  EXPECT_EQ(server_list[0].listen_address_, "0.0.0.0");
  EXPECT_EQ(server_list[0].listen_port_, "5500");
  EXPECT_EQ(server_list[0].server_name_, "example.com");

  EXPECT_EQ(server_list[1].listen_address_, "0.0.0.0");
  EXPECT_EQ(server_list[1].listen_port_, "5500");
  EXPECT_EQ(server_list[1].root_, "/tmp/www");

  EXPECT_EQ(server_list[2].listen_address_, "0.0.0.0");
  EXPECT_EQ(server_list[2].listen_port_, "5001");
  EXPECT_EQ(server_list[2].server_name_, "example.net");
}
