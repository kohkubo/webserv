#include "gtest/gtest.h"

#include "ServerConfig.hpp"
#include "util.hpp"

#define SPACES "\v\r\f\t\n "

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

TEST(server_config_test, server_name_except) {
  {
    std::string              str = "server {\n"
                                   "\n"
                                   "server_name ;\n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " ");
    ServerConfig             conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string              str = "server {\n"
                                   "\n"
                                   "server_name example.com example.net;\n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " ");
    ServerConfig             conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string              str = "server {\n"
                                   "server_name example.com;\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", " \n");
    ServerConfig             conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
}

TEST(server_config_test, parse_server_name) {
  {
    std::string              str = "server {\n"
                                   "server_name example.com;\n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", SPACES);
    ServerConfig             conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.server_name_, "example.com");
  }
  {
    std::string              str = "server {\n"
                                   "server_name    example.com    ;\n"
                                   "}\n";

    std::vector<std::string> l   = tokenize(str, SPACES "{};", SPACES);
    ServerConfig             conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.server_name_, "example.com");
  }
}
