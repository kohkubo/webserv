#include "gtest/gtest.h"

#include "config/ConfGroupMapGenerator.hpp"
#include "config/Config.hpp"
#include "event/Transaction.hpp"

TEST(transaction_test, detect_properconf) {
  ConfGroupMapGenerator conf_group_map_generator("tdata/transaction_test.conf");
  std::map<listenFd, confGroup> conf_group_map =
      conf_group_map_generator.generate();
  confGroup   conf_group = conf_group_map.begin()->second;

  std::string apple_req  = "GET / HTTP/1.1\r\n"
                           "Host: apple.com\r\n"
                           "\r\n";
  std::string orange_req = "GET / HTTP/1.1\r\n"
                           "Host: orange.net\r\n"
                           "\r\n";
  std::string banana_req = "GET / HTTP/1.1\r\n"
                           "Host: banana.com\r\n"
                           "\r\n";
  std::string peach_req  = "GET / HTTP/1.1\r\n"
                           "Host: peach.com\r\n"
                           "\r\n";
  {
    Transaction t;
    t.parse_single_request(apple_req, conf_group);
    const Config *config = t.get_conf();

    EXPECT_EQ(config->server_name_, "apple.com");
  }
  {
    Transaction t;
    t.parse_single_request(orange_req, conf_group);
    const Config *config = t.get_conf();
    EXPECT_EQ(config->server_name_, "orange.net");
  }
  {
    Transaction t;
    t.parse_single_request(banana_req, conf_group);
    const Config *config = t.get_conf();
    EXPECT_EQ(config->server_name_, "banana.com");
  }
  {
    Transaction t;
    t.parse_single_request(peach_req, conf_group);
    const Config *config = t.get_conf();
    EXPECT_EQ(config->server_name_, "apple.com");
  }
  // close all sockets
  std::map<listenFd, confGroup>::iterator it;
  for (it = conf_group_map.begin(); it != conf_group_map.end(); it++) {
    close(it->first);
  }
}