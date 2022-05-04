#include "gtest/gtest.h"

#include "config/ServerConfig.hpp"
#include "config/read_config.hpp"
#include "util/tokenize.hpp"
#include "util/util.hpp"

#define SAMPLE_CONF "../googletest/tdata/sample.conf"

TEST(server_config_test, server_exception) {
  {
    std::string  str = "server \n"
                       "}\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string  str = "server {\n"
                       "\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string  str = "server {\n"
                       "\n"
                       "server {\n"
                       "}\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
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

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string  str = "server {\n"
                       "root 80\n"
                       "}\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
}

TEST(server_config_test, parse_root) {
  std::string  str = "server {\n"
                     "root /var/fuga;\n"
                     "}\n";

  token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
  ServerConfig conf;
  conf.parse(l.begin(), l.end());
  EXPECT_EQ(conf.root_, "/var/fuga");
}

TEST(server_config_test, listen_except) {
  {
    std::string  str = "server {\n"
                       "listen 888\n"
                       "}\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string  str = "server {\n"
                       "listen ;\n"
                       "}\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
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

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.listen_port_, "888");
  }
  {
    std::string  str = "server {\n"
                       "listen 192.168.0.1;\n"
                       "}\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.listen_address_, "192.168.0.1");
  }
  {
    std::string  str = "server {\n"
                       "listen 192.168.0.1:80;\n"
                       "}\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.listen_address_, "192.168.0.1");
    EXPECT_EQ(conf.listen_port_, "80");
  }
  {
    std::string  str = "server {\n"
                       "listen localhost:80;\n"
                       "}\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.listen_address_, "localhost");
    EXPECT_EQ(conf.listen_port_, "80");
  }
}

TEST(server_config_test, server_name_except) {
  {
    std::string  str = "server {\n"
                       "\n"
                       "server_name ;\n"
                       "}\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string  str = "server {\n"
                       "\n"
                       "server_name example.com example.net;\n"
                       "}\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
  {
    std::string  str = "server {\n"
                       "server_name example.com;\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    EXPECT_THROW(conf.parse(l.begin(), l.end()),
                 ServerConfig::UnexpectedTokenException);
  }
}

TEST(server_config_test, parse_server_name) {
  {
    std::string  str = "server {\n"
                       "server_name example.com;\n"
                       "}\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.server_name_, "example.com");
  }
  {
    std::string  str = "server {\n"
                       "server_name    example.com    ;\n"
                       "}\n";

    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.server_name_, "example.com");
  }
}

TEST(server_config_test, server_group_test) {
  server_list_type  server_list  = read_config(SAMPLE_CONF);
  server_group_type server_group = create_server_group(server_list);

  EXPECT_EQ(server_group[0][0]->listen_address_, "0.0.0.0");
  EXPECT_EQ(server_group[0][0]->listen_port_, "5500");
  EXPECT_EQ(server_group[0][1]->listen_address_, "0.0.0.0");
  EXPECT_EQ(server_group[0][1]->listen_port_, "5500");
  EXPECT_EQ(server_group[1][0]->listen_address_, "0.0.0.0");
  EXPECT_EQ(server_group[1][0]->listen_port_, "5001");
  EXPECT_EQ(server_group.size(), 2);
  EXPECT_EQ(server_group[0].size(), 2);
}
