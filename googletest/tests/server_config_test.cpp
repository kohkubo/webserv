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

TEST(server_config_test, parse_string_derective_exception) {
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

TEST(server_config_test, parse_string_derective) {
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

TEST(server_config_test, parse_map_directive) {
  {
    std::string  str = "server {\n"
                       "error_page 404 /404.html;\n"
                       "error_page 500 /500.html;\n"
                       "}\n";
    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.error_pages_.size(), 2);
    EXPECT_EQ(conf.error_pages_[404], "/404.html");
    EXPECT_EQ(conf.error_pages_[500], "/500.html");
  }
}

TEST(server_config_test, parse_boolean_directive) {
  {
    std::string  str = "server {\n"
                       "autoindex on;\n"
                       "}\n";
    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.autoindex_, true);
  }
  {
    std::string  str = "server {\n"
                       "autoindex off;\n"
                       "}\n";
    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.autoindex_, false);
  }
}

TEST(server_config_test, parse_sizet_directive) {
  {
    std::string  str = "server {\n"
                       "client_max_body_size 1000;\n"
                       "}\n";
    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    EXPECT_EQ(conf.client_max_body_size_, 1000);
  }
}

TEST(server_config_test, parse_vector_directive) {
  {
    std::string  str = "server {\n"
                       "limit_except GET POST;\n"
                       "}\n";
    token_vector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    ServerConfig conf;
    conf.parse(l.begin(), l.end());
    // EXPECT_EQ(conf.limit_except_.size(), 2);
    // EXPECT_EQ(conf.limit_except_[0], "GET");
    // EXPECT_EQ(conf.limit_except_[1], "POST");
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
