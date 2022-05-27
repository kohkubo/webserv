#include "gtest/gtest.h"

#include "config/ConfGroupMapGenerator.hpp"
#include "config/Config.hpp"
#include "utils/tokenize.hpp"

#define SAMPLE_CONF "../googletest/tdata/sample.conf"

TEST(server_config_test, server_exception) {
  {
    std::string str = "server \n"
                      "}\n";

    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    EXPECT_EXIT({ Config config(l.begin(), l.end()); },
                ::testing::ExitedWithCode(EXIT_FAILURE), "");
  }
  {
    std::string str = "server {\n"
                      "\n";

    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    EXPECT_EXIT({ Config config(l.begin(), l.end()); },
                ::testing::ExitedWithCode(EXIT_FAILURE), "");
  }
  {
    std::string str = "server {\n"
                      "\n"
                      "server {\n"
                      "}\n";

    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    EXPECT_EXIT({ Config config(l.begin(), l.end()); },
                ::testing::ExitedWithCode(EXIT_FAILURE), "");
  }
}

TEST(server_config_test, parse_string_derective_exception) {
  {
    std::string str = "server {\n"
                      "location / {\n"
                      "root ;\n"
                      "}\n"
                      "}\n";

    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    EXPECT_EXIT({ Config config(l.begin(), l.end()); },
                ::testing::ExitedWithCode(EXIT_FAILURE), "");
  }
  {
    std::string str = "server {\n"
                      "location / {\n"
                      "root 80\n"
                      "}\n"
                      "}\n";

    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    EXPECT_EXIT({ Config config(l.begin(), l.end()); },
                ::testing::ExitedWithCode(EXIT_FAILURE), "");
  }
}

TEST(server_config_test, parse_string_derective) {
  {
    std::string str = "server {\n"
                      "server_name example.com;\n"
                      "location / {\n"
                      "root /var/www/html;\n"
                      "}\n"
                      "}\n";

    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(l.begin(), l.end());
    EXPECT_EQ(config.server_name_, "example.com");
  }
  {
    std::string str = "server {\n"
                      "server_name    example.com    ;\n"
                      "location / {\n"
                      "root /var/www/html;\n"
                      "}\n"
                      "}\n";

    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(l.begin(), l.end());
    EXPECT_EQ(config.server_name_, "example.com");
  }
}

TEST(server_config_test, listen_except) {
  {
    std::string str = "server {\n"
                      "listen 888\n"
                      "location / {\n"
                      "root /var/www/html;\n"
                      "}\n"
                      "}\n";

    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    EXPECT_EXIT({ Config config(l.begin(), l.end()); },
                ::testing::ExitedWithCode(EXIT_FAILURE), "");
  }
  {
    std::string str = "server {\n"
                      "listen ;\n"
                      "location / {\n"
                      "root /var/www/html;\n"
                      "}\n"
                      "}\n";

    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    EXPECT_EXIT({ Config config(l.begin(), l.end()); },
                ::testing::ExitedWithCode(EXIT_FAILURE), "");
  }
}

TEST(server_config_test, parse_listen) {
  {
    std::string str = "server {\n"
                      "listen 888;\n"
                      "location / {\n"
                      "root /var/www/html;\n"
                      "}\n"
                      "}\n";

    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(l.begin(), l.end());
    EXPECT_EQ(config.listen_port_, "888");
  }
  {
    std::string str = "server {\n"
                      "listen 192.168.0.1;\n"
                      "location / {\n"
                      "root /var/www/html;\n"
                      "}\n"
                      "}\n";

    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(l.begin(), l.end());
    EXPECT_EQ(config.listen_address_, "192.168.0.1");
  }
  {
    std::string str = "server {\n"
                      "listen 192.168.0.1:80;\n"
                      "location / {\n"
                      "root /var/www/html;\n"
                      "}\n"
                      "}\n";

    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(l.begin(), l.end());
    EXPECT_EQ(config.listen_address_, "192.168.0.1");
    EXPECT_EQ(config.listen_port_, "80");
  }
  {
    std::string str = "server {\n"
                      "listen localhost:80;\n"
                      "location / {\n"
                      "root /var/www/html;\n"
                      "}\n"
                      "}\n";

    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(l.begin(), l.end());
    EXPECT_EQ(config.listen_address_, "localhost");
    EXPECT_EQ(config.listen_port_, "80");
  }
}

TEST(server_config_test, parse_map_directive) {
  {
    std::string str = "server {\n"
                      "error_page 404 /404.html;\n"
                      "error_page 500 /500.html;\n"
                      "location / {\n"
                      "root /var/www/html;\n"
                      "}\n"
                      "}\n";
    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(l.begin(), l.end());
    EXPECT_EQ(config.error_pages_.size(), 2);
    EXPECT_EQ(config.error_pages_[404], "/404.html");
    EXPECT_EQ(config.error_pages_[500], "/500.html");
  }
}

TEST(server_config_test, parse_boolean_directive) {
  {
    std::string str = "server {\n"
                      "location / {\n"
                      "autoindex on;\n"
                      "}\n"
                      "}\n";
    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(l.begin(), l.end());
    EXPECT_EQ(config.locations_[0].autoindex_, true);
  }
  {
    std::string str = "server {\n"
                      "location / {\n"
                      "autoindex off;\n"
                      "}\n"
                      "}\n";
    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(l.begin(), l.end());
    EXPECT_EQ(config.locations_[0].autoindex_, false);
  }
}

TEST(server_config_test, parse_sizet_directive) {
  {
    std::string str = "server {\n"
                      "client_max_body_size 1000;\n"
                      "location / {\n"
                      "root /var/www;\n"
                      "}\n"
                      "}\n";
    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(l.begin(), l.end());
    EXPECT_EQ(config.client_max_body_size_, 1000);
  }
}

TEST(server_config_test, parse_vector_directive) {
  {
    std::string str = "server {\n"
                      "location / {\n"
                      "limit_except GET POST;\n"
                      "}\n"
                      "}\n";
    tokenVector l   = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(l.begin(), l.end());
    EXPECT_EQ(config.locations_[0].limit_except_.size(), 2);
    EXPECT_EQ(config.locations_[0].limit_except_[0], "GET");
    EXPECT_EQ(config.locations_[0].limit_except_[1], "POST");
  }
}

TEST(server_config_test, socket_list_test) {
  ConfGroupMapGenerator         config_map_generator(SAMPLE_CONF);
  std::map<listenFd, confGroup> conf_group_map =
      config_map_generator.generate();

  EXPECT_EQ(conf_group_map.size(), 2);

  std::map<listenFd, confGroup>::iterator it = conf_group_map.begin();
  EXPECT_EQ((it->second).size(), 2);
  EXPECT_EQ((it->second)[0]->listen_address_, "0.0.0.0");
  EXPECT_EQ((it->second)[0]->listen_port_, "5500");
  EXPECT_EQ((it->second)[1]->listen_address_, "0.0.0.0");
  EXPECT_EQ((it->second)[1]->listen_port_, "5500");
  it++;
  EXPECT_EQ((it->second).size(), 1);
  EXPECT_EQ((it->second)[0]->listen_address_, "0.0.0.0");
  EXPECT_EQ((it->second)[0]->listen_port_, "5001");
  // close all sockets
  for (it = conf_group_map.begin(); it != conf_group_map.end(); it++) {
    close(it->first);
  }
}
