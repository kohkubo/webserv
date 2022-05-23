#include "gtest/gtest.h"

#include "config/Config.hpp"
#include "config/create_listen_fd_map.hpp"
#include "event/Transaction.hpp"

TEST(transaction_test, detect_properconf) {
  serverList server_list = read_config("tdata/transaction_test.conf");
  confGroup  conf_group;
  for (serverList::iterator it = server_list.begin(); it != server_list.end();
       it++) {
    conf_group.push_back(&(*it));
  }

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
    t.parse_header(apple_req, conf_group);
    const Config *conf = t.get_conf();
    EXPECT_EQ(conf->server_name_, "apple.com");
  }
  {
    Transaction t;
    t.parse_header(orange_req, conf_group);
    const Config *conf = t.get_conf();
    EXPECT_EQ(conf->server_name_, "orange.net");
  }
  {
    Transaction t;
    t.parse_header(banana_req, conf_group);
    const Config *conf = t.get_conf();
    EXPECT_EQ(conf->server_name_, "banana.com");
  }
  {
    Transaction t;
    t.parse_header(peach_req, conf_group);
    const Config *conf = t.get_conf();
    EXPECT_EQ(conf->server_name_, "apple.com");
  }
}