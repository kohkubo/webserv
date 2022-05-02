#include "gtest/gtest.h"
#include "http/method.hpp"
#include "http/HttpMessage.hpp"
#include "http/const_response_key_map.hpp"
#include "http/const_status_phrase.hpp"
#include "http/response.hpp"
#include "config/ServerConfig.hpp"

TEST(http_test, create_response) {
  ServerConfig *server_config = new ServerConfig();
  server_config->root_ = "../html/";
  HttpMessage      request_message;
  request_message.method_ = GET;
  request_message.url_ = "/";
  request_message.version_ = "HTTP/1.1";
  request_message.host_ = "localhost";
  std::string expect             = "HTTP/1.1 200 OK\r\n"
                                   "Content-Length: 127\r\n"
                                   "Content-Type: text/html\r\n"
                                   "Connection: close\r\n"
                                   "\r\n"
                                   "<!DOCTYPE html>\n"
                                   "<html>\n"
                                   "    <head>\n"
                                   "        <title>Basic Web Page</title>\n"
                                   "    </head>\n"
                                   "    <body>\n"
                                   "Hello World!\n"
                                   "    </body>\n"
                                   "</html>";
  EXPECT_EQ(create_response(server_config, request_message), expect);
}

#define TEST_FILE          "../googletest/tdata/test.txt"
#define TEST_CONTENT       "test"
#define EMPTY_FILE         "../googletest/tdata/empty.txt"
#define NO_PERMISSION_FILE "../googletest/tdata/no_permission.txt"
#define NO_SUCH_FILE       "no such file"

TEST(http_test, method_get) {
  ServerConfig *server_config = new ServerConfig();
  server_config->root_ = "./tdata/";
  server_config->index_ = "test.txt";
  HttpMessage request_message;
  http_message_map response_message;
  request_message.url_ = "/";
  response_message     = method_get(server_config, request_message);
  EXPECT_EQ(response_message[STATUS_PHRASE], STATUS_200_PHRASE);
  EXPECT_EQ(response_message[BODY], TEST_CONTENT);
  EXPECT_EQ(response_message[CONTENT_LEN],
            std::to_string((strlen(TEST_CONTENT))));
}

// 今はget()の中でNOT_FOUNDページを挿入するようになっているため
// テスト側からNOT_FOUNDページの中身について確認するのが難しいのでボディのテスト保留
TEST(http_test, target_file_not_exist) {
  const ServerConfig *server_config = new ServerConfig();
  HttpMessage request_message;
  http_message_map response_message;

  response_message     = method_get(server_config, request_message);
  EXPECT_EQ(response_message[STATUS_PHRASE], STATUS_404_PHRASE);
  // EXPECT_EQ(response_message[BODY], TEST_CONTENT);
  // EXPECT_EQ(response_message[CONTENT_LEN],
  // std::to_string((strlen(TEST_CONTENT))));
}
