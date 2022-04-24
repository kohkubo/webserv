#include <vector>
#include "ServerConfig.hpp"
#include "Webserv.hpp"
#include "gtest/gtest.h"

#define SAMPLE_CONF "../googletest/tdata/sample.conf"

TEST(read_config_test, simple_test) {
  std::vector<ServerConfig> server_list = read_config(SAMPLE_CONF);

  EXPECT_EQ(server_list[0].listen_address_, std::string("0.0.0.0"));
  EXPECT_EQ(server_list[0].listen_port_, std::string("5500"));

  EXPECT_EQ(server_list[1].listen_address_, std::string("0.0.0.0"));
  EXPECT_EQ(server_list[1].listen_port_, std::string("5500"));

  EXPECT_EQ(server_list[2].listen_address_, std::string("0.0.0.0"));
  EXPECT_EQ(server_list[2].listen_port_, std::string("5001"));
}
