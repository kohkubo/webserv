#include "gtest/gtest.h"

#include "config/Config.hpp"
#include "event/Server.hpp"
#include "utils/tokenize.hpp"

#define SAMPLE_CONF "../googletest/tdata/sample.conf"

TEST(server_config_test, server_exception) {
  {
    std::string str          = "server \n"
                               "}\n";

    tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    EXPECT_EXIT({ Config config(token_vector.begin(), token_vector.end()); },
                ::testing::ExitedWithCode(EXIT_FAILURE), "");
  }
  {
    std::string str          = "server {\n"
                               "\n";

    tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    EXPECT_EXIT({ Config config(token_vector.begin(), token_vector.end()); },
                ::testing::ExitedWithCode(EXIT_FAILURE), "");
  }
  {
    std::string str          = "server {\n"
                               "\n"
                               "server {\n"
                               "}\n";

    tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    EXPECT_EXIT({ Config config(token_vector.begin(), token_vector.end()); },
                ::testing::ExitedWithCode(EXIT_FAILURE), "");
  }
}

TEST(server_config_test, parse_string_derective_exception) {
  {
    std::string str          = "server {\n"
                               "    location / {\n"
                               "        root ;\n"
                               "    }\n"
                               "}\n";

    tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    EXPECT_EXIT({ Config config(token_vector.begin(), token_vector.end()); },
                ::testing::ExitedWithCode(EXIT_FAILURE), "");
  }
  {
    std::string str          = "server {\n"
                               "    location / {\n"
                               "        root 80/\n"
                               "    }\n"
                               "}\n";

    tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    EXPECT_EXIT({ Config config(token_vector.begin(), token_vector.end()); },
                ::testing::ExitedWithCode(EXIT_FAILURE), "");
  }
}

TEST(server_config_test, parse_string_derective) {
  {
    std::string str          = "server {\n"
                               "    listen 0.0.0.0:50001;\n"
                               "    server_name example.com;\n"
                               "    location / {\n"
                               "        root /var/www/html/;\n"
                               "    }\n"
                               "}\n";

    tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(token_vector.begin(), token_vector.end());
    EXPECT_EQ(config.server_name_, "example.com");
  }
  {
    std::string str          = "server {\n"
                               "    listen 0.0.0.0:50001;\n"
                               "    server_name    example.com    ;\n"
                               "    location / {\n"
                               "        root /var/www/html/;\n"
                               "    }\n"
                               "}\n";

    tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(token_vector.begin(), token_vector.end());
    EXPECT_EQ(config.server_name_, "example.com");
  }
}

TEST(server_config_test, listen_exception) {
  std::string str          = "server {\n"
                             "    listen 888\n"
                             "    location / {\n"
                             "        root /var/www/html/;\n"
                             "    }\n"
                             "}\n";

  tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
  EXPECT_EXIT({ Config config(token_vector.begin(), token_vector.end()); },
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(server_config_test, listen_exception2) {
  std::string str          = "server {\n"
                             "    listen ;\n"
                             "    location / {\n"
                             "        root /var/www/html/;\n"
                             "    }\n"
                             "}\n";

  tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
  EXPECT_EXIT({ Config config(token_vector.begin(), token_vector.end()); },
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(server_config_test, listen_exception3) {
  std::string str          = "server {\n"
                             "    listen hogegoe3:4933;\n"
                             "    location / {\n"
                             "        root /var/www/html/;\n"
                             "    }\n"
                             "}\n";

  tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
  EXPECT_EXIT({ Config config(token_vector.begin(), token_vector.end()); },
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(server_config_test, listen_exception4) {
  std::string str          = "server {\n"
                             "    listen 4:::3dd4;\n"
                             "    location / {\n"
                             "        root /var/www/html/;\n"
                             "    }\n"
                             "}\n";

  tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
  EXPECT_EXIT({ Config config(token_vector.begin(), token_vector.end()); },
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(server_config_test, parse_map_directive) {
  {
    std::string str          = "server {\n"
                               "    listen 0.0.0.0:50001;\n"
                               "    error_page 404 /404.html;\n"
                               "    error_page 500 /500.html;\n"
                               "    location / {\n"
                               "        root /var/www/html/;\n"
                               "    }\n"
                               "}\n";
    tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config      config(token_vector.begin(), token_vector.end());
    EXPECT_EQ(config.error_pages_.size(), static_cast<std::size_t>(2));
    EXPECT_EQ(config.error_pages_[404], "/404.html");
    EXPECT_EQ(config.error_pages_[500], "/500.html");
  }
}

TEST(server_config_test, parse_map_directive_exception) {
  {
    std::string str          = "server {\n"
                               "    listen 0.0.0.0:50001;\n"
                               "    error_page -404 /404.html;\n"
                               "    location / {\n"
                               "        root /var/www/html/;\n"
                               "    }\n"
                               "}\n";
    tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    EXPECT_EXIT({ Config config(token_vector.begin(), token_vector.end()); },
                ::testing::ExitedWithCode(EXIT_FAILURE), "");
  }
}

TEST(server_config_test, parse_boolean_directive) {
  {
    std::string     str          = "server {\n"
                                   "   listen 0.0.0.0:50001;\n"
                                   "    location / {\n"
                                   "        autoindex on;\n"
                                   "    }\n"
                                   "}\n";
    tokenVector     token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config          config(token_vector.begin(), token_vector.end());
    const Location *location = config.locations_.select_location("/");
    EXPECT_EQ(location->autoindex_, true);
  }
  {
    std::string     str          = "server {\n"
                                   "   listen 0.0.0.0:50001;\n"
                                   "    location / {\n"
                                   "        autoindex off;\n"
                                   "    }\n"
                                   "}\n";
    tokenVector     token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config          config(token_vector.begin(), token_vector.end());
    const Location *location = config.locations_.select_location("/");
    EXPECT_EQ(location->autoindex_, false);
  }
}

TEST(server_config_test, parse_size_directive) {
  std::string str          = "server {\n"
                             "   listen 0.0.0.0:50001;\n"
                             "    client_max_body_size 1000;\n"
                             "    location / {\n"
                             "        root /var/www/;\n"
                             "    }\n"
                             "}\n";
  tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
  Config      config(token_vector.begin(), token_vector.end());
  EXPECT_EQ(config.client_max_body_size_, static_cast<std::size_t>(1000));
}

TEST(server_config_test, parse_size_directive_exception) {
  std::string str          = "server {\n"
                             "   listen 0.0.0.0:50001;\n"
                             "    client_max_body_size -1000;\n"
                             "    location / {\n"
                             "        root /var/www/;\n"
                             "    }\n"
                             "}\n";
  tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
  EXPECT_EXIT({ Config config(token_vector.begin(), token_vector.end()); },
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(server_config_test, parse_size_directive_exception2) {
  std::string str          = "server {\n"
                             "   listen 0.0.0.0:50001;\n"
                             "    client_max_body_size hoge;\n"
                             "    location / {\n"
                             "        root /var/www/;\n"
                             "    }\n"
                             "}\n";
  tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
  EXPECT_EXIT({ Config config(token_vector.begin(), token_vector.end()); },
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(server_config_test, parse_size_directive_exception3) {
  std::string str          = "server {\n"
                             "   listen 0.0.0.0:50001;\n"
                             "    client_max_body_size 123hoge;\n"
                             "    location / {\n"
                             "        root /var/www/;\n"
                             "    }\n"
                             "}\n";
  tokenVector token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
  EXPECT_EXIT({ Config config(token_vector.begin(), token_vector.end()); },
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(server_config_test, parse_vector_directive) {
  {
    std::string     str          = "server {\n"
                                   "   listen 0.0.0.0:50001;\n"
                                   "    location / {\n"
                                   "        available_methods GET POST;\n"
                                   "    }\n"
                                   "}\n";
    tokenVector     token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config          config(token_vector.begin(), token_vector.end());
    const Location *location = config.locations_.select_location("/");
    EXPECT_EQ(location->available_methods_.get_, true);
    EXPECT_EQ(location->available_methods_.post_, true);
    EXPECT_EQ(location->available_methods_.delete_, false);
  }
}

TEST(server_config_test, parse_location_directive) {
  {
    std::string     str          = "server {\n"
                                   "   listen 0.0.0.0:50001;\n"
                                   "    location / {\n"
                                   "        root /var/www/;\n"
                                   "        available_methods GET POST;\n"
                                   "    }\n"
                                   "    location /dir1 {\n"
                                   "        root /var/www/;\n"
                                   "        available_methods GET POST;\n"
                                   "    }\n"
                                   "}\n";
    tokenVector     token_vector = tokenize(str, CONFIG_DELIMITER, CONFIG_SKIP);
    Config          config(token_vector.begin(), token_vector.end());
    const Location *location = config.locations_.select_location("/");
    EXPECT_EQ(location->available_methods_.get_, true);
    EXPECT_EQ(location->available_methods_.post_, true);
    EXPECT_EQ(location->available_methods_.delete_, false);
    location = config.locations_.select_location("/dir1");
    EXPECT_EQ(location->available_methods_.get_, true);
    EXPECT_EQ(location->available_methods_.post_, true);
    EXPECT_EQ(location->available_methods_.delete_, false);
  }
}
