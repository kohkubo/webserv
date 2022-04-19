#include "http.hpp"
#include "gtest/gtest.h"

TEST(http_test, create_response) {
  http_message_map      request_message;
  request_message[METHOD] = "GET";
  request_message[URL] = "./../html/index.html";
  request_message[VERSION] = "HTTP/1.1";
  request_message[HOST] = "localhost:5001";
  request_message[USERAGENT] = "curl/7.79.1";
  request_message[ACCEPT] = "*/*";
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
  EXPECT_EQ(create_response(request_message), expect);
}

#define TEST_FILE          "../googletest/tdata/test.txt"
#define TEST_CONTENT       "test"
#define EMPTY_FILE         "../googletest/tdata/empty.txt"
#define NO_PERMISSION_FILE "../googletest/tdata/no_permission.txt"
#define NO_SUCH_FILE       "no such file"

TEST(http_test, method_get) {
  http_message_map request_message;
  http_message_map response_message;

  request_message[URL] = TEST_FILE;
  response_message     = method_get(request_message);
  EXPECT_EQ(response_message[STATUS], STATUS_OK);
  EXPECT_EQ(response_message[PHRASE], PHRASE_STATUS_OK);
  EXPECT_EQ(response_message[BODY], TEST_CONTENT);
  EXPECT_EQ(response_message[CONTENT_LEN],
            std::to_string((strlen(TEST_CONTENT))));
}

// 今はget()の中でNOT_FOUNDページを挿入するようになっているため
// テスト側からNOT_FOUNDページの中身について確認するのが難しいのでボディのテスト保留
TEST(http_test, target_file_not_exist) {
  http_message_map request_message;
  http_message_map response_message;

  request_message[URL] = NO_SUCH_FILE;
  response_message     = method_get(request_message);
  EXPECT_EQ(response_message[STATUS], STATUS_NOTFOUND);
  EXPECT_EQ(response_message[PHRASE], PHRASE_STATUS_NOTFOUND);
  // EXPECT_EQ(response_message[BODY], TEST_CONTENT);
  // EXPECT_EQ(response_message[CONTENT_LEN],
  // std::to_string((strlen(TEST_CONTENT))));
}
