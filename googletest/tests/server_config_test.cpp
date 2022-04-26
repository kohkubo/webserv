#include "gtest/gtest.h"

#include "ServerConfig.hpp"
#include "Webserv.hpp"
#include "util.hpp"

#define SAMPLE_CONF "../googletest/tdata/sample.conf"
#define SPACES      "\v\r\f\t\n "

TEST(server_config_test, server_exception) {
  {
    std::string              str = "server \n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " ");
    ServerConfig             conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string              str = "server {\n"
                                   "\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " ");
    ServerConfig             conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string              str = "server {\n"
                                   "\n"
                                   "server {\n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " ");
    ServerConfig             conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
}

TEST(server_config_test, root_exception) {
  {
    std::string              str = "server {\n"
                                   "root ;\n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " ");
    ServerConfig             conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string              str = "server {\n"
                                   "root 80\n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " ");
    ServerConfig             conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
}

TEST(server_config_test, parse_root) {
  std::string              str = "server {\n"
                                 "root /var/fuga;\n"
                                 "}\n";

  std::vector<std::string> l   = tokenize(str, SPACES "{};", " \n");
  ServerConfig             conf;
  conf.parse(l.begin(), l.end());
  EXPECT_EQ(conf.root_, "/var/fuga");
}

TEST(server_config_test, listen_except) {
  {
    std::string              str = "server {\n"
                                   "listen 888\n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " \n");
    ServerConfig             conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string              str = "server {\n"
                                   "listen ;\n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " \n");
    ServerConfig             conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
}

TEST(server_config_test, parse_listen) {
  {
    std::string              str = "server {\n"
                                   "listen 888;\n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " \n");
    ServerConfig             conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.listen_port_, "888");
  }
  {
    std::string              str = "server {\n"
                                   "listen 192.168.0.1;\n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " \n");
    ServerConfig             conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.listen_address_, "192.168.0.1");
  }
  {
    std::string              str = "server {\n"
                                   "listen 192.168.0.1:80;\n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " \n");
    ServerConfig             conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.listen_address_, "192.168.0.1");
    EXPECT_EQ(conf.listen_port_, "80");
  }
  {
    std::string              str = "server {\n"
                                   "listen localhost:80;\n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " \n");
    ServerConfig             conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.listen_address_, "localhost");
    EXPECT_EQ(conf.listen_port_, "80");
  }
}

TEST(server_config_test, server_group_test) {
  std::vector<ServerConfig> server_list  = read_config(SAMPLE_CONF);
  server_group_type         server_group = create_server_group(server_list);

  EXPECT_EQ(server_group[0][0].listen_address_, "0.0.0.0");
  EXPECT_EQ(server_group[0][0].listen_port_, "5500");
  EXPECT_EQ(server_group[0][1].listen_address_, "0.0.0.0");
  EXPECT_EQ(server_group[0][1].listen_port_, "5500");
  EXPECT_EQ(server_group[1][0].listen_address_, "0.0.0.0");
  EXPECT_EQ(server_group[1][0].listen_port_, "5001");
  EXPECT_EQ(server_group.size(), 2);
  EXPECT_EQ(server_group[0].size(), 2);
}
