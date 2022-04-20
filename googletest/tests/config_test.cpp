#include "gtest/gtest.h"

#include "config/Config.hpp"
#include "config/ServerConfig.hpp"

#define SPACES "\v\r\f\t\n "

TEST(server_config_test, server_exception) {
  {
    std::string  str = "server \n"
                       "}\n";

    Tokens       l(str, SPACES "{};", " ");
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string  str = "server {\n"
                       "\n";

    Tokens       l(str, SPACES "{};", " ");
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string  str = "server {\n"
                       "\n"
                       "server {\n"
                       "}\n";

    Tokens       l(str, SPACES "{};", " ");
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
}

TEST(server_config_test, root_exception) {
  {
    std::string  str = "server {\n"
                       "root ;\n"
                       "}\n";

    Tokens       l(str, SPACES "{};", " ");
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string  str = "server {\n"
                       "root 80\n"
                       "}\n";

    Tokens       l(str, SPACES "{};", " ");
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
}

TEST(server_config_test, parse_root) {
  std::string  str = "server {\n"
                     "root /var/fuga;\n"
                     "}\n";

  Tokens       l(str, SPACES "{};", " \n");
  ServerConfig conf;
  conf.parse(l.begin(), l.end());
  EXPECT_EQ(conf.root_, "/var/fuga");
}

TEST(server_config_test, listen_except) {
  {
    std::string  str = "server {\n"
                       "listen 888\n"
                       "}\n";

    Tokens       l(str, SPACES "{};", " \n");
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string  str = "server {\n"
                       "listen ;\n"
                       "}\n";

    Tokens       l(str, SPACES "{};", " \n");
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
}

TEST(server_config_test, parse_listen) {
  {
    std::string  str = "server {\n"
                       "listen 888;\n"
                       "}\n";

    Tokens       l(str, SPACES "{};", " \n");
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.listen_port_, "888");
  }
  {
    std::string  str = "server {\n"
                       "listen 192.168.0.1;\n"
                       "}\n";

    Tokens       l(str, SPACES "{};", " \n");
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.listen_ip_, "192.168.0.1");
  }
  {
    std::string  str = "server {\n"
                       "listen 192.168.0.1:80;\n"
                       "}\n";

    Tokens       l(str, SPACES "{};", " \n");
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.listen_ip_, "192.168.0.1");
    EXPECT_EQ(conf.listen_port_, "80");
  }
  {
    std::string  str = "server {\n"
                       "listen localhost:80;\n"
                       "}\n";

    Tokens       l(str, SPACES "{};", " \n");
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.listen_host_, "localhost");
    EXPECT_EQ(conf.listen_port_, "80");
  }
}
