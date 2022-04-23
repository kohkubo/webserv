#include "ServerConfig.hpp"
#include "event.hpp"
#include "gtest/gtest.h"

TEST(event_test, is_same_socket) {
  ServerConfig x;
  ServerConfig y;

  x.listen_address_ = "0.0.0.0";
  x.listen_port_    = "80";
  y.listen_address_ = "0.0.0.0";
  y.listen_port_    = "80";
  EXPECT_TRUE(is_same_socket(x, y));
  y.listen_address_ = "0.0.0.1";
  EXPECT_FALSE(is_same_socket(x, y));
  y.listen_address_ = "0.0.0.0";
  y.listen_port_    = "81";
  EXPECT_FALSE(is_same_socket(x, y));
  y.listen_port_ = "80";
  EXPECT_TRUE(is_same_socket(x, y));
}