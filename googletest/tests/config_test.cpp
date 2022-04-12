#include "gtest/gtest.h"

#include "config/Config.hpp"
#include "config/ServerConfig.hpp"

#define SPACES "\v\r\f\t\n "

TEST(server_config_test, server_exception) {
  {
    std::string str = "server \n"
                      "}\n";

    Lexer        l(str, SPACES "{};");
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string str = "server {\n"
                      "\n";

    Lexer        l(str, SPACES "{};");
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string str = "server {\n"
                      "\n"
                      "server {\n"
                      "}\n";

    Lexer        l(str, SPACES "{};");
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
}

TEST(server_config_test, root_exception) {
  {
    std::string str = "server {\n"
                      "root ;\n"
                      "}\n";

    Lexer        l(str, SPACES "{};");
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string str = "server {\n"
                      "root 80\n"
                      "}\n";

    Lexer        l(str, SPACES "{};");
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
}

TEST(server_config_test, parse_root) {
  std::string str = "server {\n"
                    "root /var/fuga;\n"
                    "}\n";

  Lexer        l(str, SPACES "{};");
  ServerConfig conf;
  conf.parse(l.begin(), l.end());
  EXPECT_EQ(conf.root_, "/var/fuga");
}