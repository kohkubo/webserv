#include "gtest/gtest.h"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "config/Config.hpp"
#include "http/const/const_html_filename.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/method/method_utils.hpp"
#include "http/method/method.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/response.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

TEST(http_test, create_response_info_get_normal) {
  std::string expect        = "HTTP/1.1 200 OK\r\n"
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
  Config      server_config = Config();
  server_config.root_       = "../html";
  server_config.index_      = "index.html";
  RequestInfo request_info;
  request_info.method_  = GET;
  request_info.target_  = "/";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  http_message_map response_info =
      create_response_info(server_config, request_info);

  EXPECT_EQ(response_info[STATUS_PHRASE], STATUS_200_PHRASE);
  EXPECT_EQ(response_info[PATH], "../html/index.html");
  EXPECT_EQ(response_info[VERSION], VERSION_HTTP);
  EXPECT_EQ(response_info[CONNECTION], CONNECTION_CLOSE);

  std::string response_message = make_message_string(response_info);

  EXPECT_EQ(response_message, expect);
}

TEST(http_test, create_response_info_get_404) {
  Config server_config = Config();
  server_config.root_  = "../html";
  server_config.index_ = "index.html";
  RequestInfo request_info;
  request_info.method_  = GET;
  request_info.target_  = "/hogehoge.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  http_message_map response_info =
      create_response_info(server_config, request_info);
  // testerとwebservが実行される階層が違うため、constの相対パスが想定とずれる。

  EXPECT_EQ(response_info[STATUS_PHRASE], STATUS_404_PHRASE);
  EXPECT_EQ(response_info[PATH], NOT_FOUND_PAGE);
  EXPECT_EQ(response_info[VERSION], VERSION_HTTP);
  EXPECT_EQ(response_info[CONNECTION], CONNECTION_CLOSE);
}

TEST(http_test, create_response_info_get_403) {
  Config server_config = Config();
  server_config.root_  = "../html";
  server_config.index_ = "index.html";
  RequestInfo request_info;
  request_info.method_  = GET;
  request_info.target_  = "/000.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  system("chmod 000 ../html/000.html");

  http_message_map response_info =
      create_response_info(server_config, request_info);

  system("chmod 644 ../html/000.html");

  EXPECT_EQ(response_info[STATUS_PHRASE], STATUS_403_PHRASE);
  EXPECT_EQ(response_info[PATH], FORBIDDEN_PAGE);
  EXPECT_EQ(response_info[VERSION], VERSION_HTTP);
  EXPECT_EQ(response_info[CONNECTION], CONNECTION_CLOSE);
}

TEST(http_test, create_response_info_delete_normal) {
  std::string expect        = "HTTP/1.1 204 No Content\r\n"
                              "Connection: close\r\n\r\n";
  Config      server_config = Config();
  server_config.root_       = "../html";
  server_config.index_      = "index.html";
  RequestInfo request_info;
  request_info.method_  = DELETE;
  request_info.target_  = "/delete_target.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  system("touch ../html/delete_target.html");

  http_message_map response_info =
      create_response_info(server_config, request_info);

  EXPECT_EQ(response_info[STATUS_PHRASE], STATUS_204_PHRASE);
  EXPECT_EQ(response_info[VERSION], VERSION_HTTP);
  EXPECT_EQ(response_info[CONNECTION], CONNECTION_CLOSE);

  std::string response_message = make_message_string(response_info);

  EXPECT_EQ(response_message, expect);
}

TEST(http_test, create_response_info_delete_404) {
  Config server_config = Config();
  server_config.root_  = "../html";
  server_config.index_ = "index.html";
  RequestInfo request_info;
  request_info.method_  = DELETE;
  request_info.target_  = "/delete_target.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  http_message_map response_info =
      create_response_info(server_config, request_info);

  EXPECT_EQ(response_info[STATUS_PHRASE], STATUS_404_PHRASE);
  EXPECT_EQ(response_info[PATH], NOT_FOUND_PAGE);
  EXPECT_EQ(response_info[VERSION], VERSION_HTTP);
  EXPECT_EQ(response_info[CONNECTION], CONNECTION_CLOSE);
}

TEST(http_test, create_response_info_delete_403) {
  Config server_config = Config();
  server_config.root_  = "../html";
  server_config.index_ = "index.html";
  RequestInfo request_info;
  request_info.method_  = DELETE;
  request_info.target_  = "/000.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  system("chmod 000 ../html/000.html");

  http_message_map response_info =
      create_response_info(server_config, request_info);

  system("chmod 644 ../html/000.html");

  EXPECT_EQ(response_info[STATUS_PHRASE], STATUS_403_PHRASE);
  EXPECT_EQ(response_info[PATH], FORBIDDEN_PAGE);
  EXPECT_EQ(response_info[VERSION], VERSION_HTTP);
  EXPECT_EQ(response_info[CONNECTION], CONNECTION_CLOSE);
}

TEST(http_test, create_response_info_delete_400) {
  Config server_config = Config();
  server_config.root_  = "../html";
  server_config.index_ = "index.html";
  RequestInfo request_info;
  request_info.method_         = DELETE;
  request_info.target_         = "/hogehoge.html";
  request_info.version_        = "HTTP/1.1";
  request_info.host_           = "localhost";
  request_info.content_length_ = 8;

  http_message_map response_info =
      create_response_info(server_config, request_info);

  EXPECT_EQ(response_info[STATUS_PHRASE], STATUS_400_PHRASE);
  EXPECT_EQ(response_info[PATH], BAD_REQUEST_PAGE);
  EXPECT_EQ(response_info[VERSION], VERSION_HTTP);
  EXPECT_EQ(response_info[CONNECTION], CONNECTION_CLOSE);
}

#define TEST_FILE          "../googletest/tdata/test.txt"
#define TEST_CONTENT       "test"
#define EMPTY_FILE         "../googletest/tdata/empty.txt"
#define NO_PERMISSION_FILE "../googletest/tdata/no_permission.txt"
#define NO_SUCH_FILE       "no such file"

TEST(http_test, method_get) {
  Config server_config = Config();
  server_config.root_  = "./tdata/";
  server_config.index_ = "test.txt";
  RequestInfo      request_info;
  http_message_map response_info;
  request_info.target_ = "/";
  request_info.host_   = "localhost";
  response_info        = method_get(server_config, request_info);
  set_response_body(response_info);
  EXPECT_EQ(response_info[STATUS_PHRASE], STATUS_200_PHRASE);
  EXPECT_EQ(response_info[BODY], TEST_CONTENT);
  EXPECT_EQ(response_info[CONTENT_LEN], std::to_string((strlen(TEST_CONTENT))));
}

// 今はget()の中でNOT_FOUNDページを挿入するようになっているため
// テスト側からNOT_FOUNDページの中身について確認するのが難しいのでボディのテスト保留
TEST(http_test, target_file_not_exist) {
  Config           server_config = Config();

  RequestInfo      request_info;
  http_message_map response_info;

  request_info.target_ = "/no_such_file";
  request_info.host_   = "localhost";

  response_info        = method_get(server_config, request_info);
  EXPECT_EQ(response_info[STATUS_PHRASE], STATUS_404_PHRASE);
  // EXPECT_EQ(response_message[BODY], TEST_CONTENT);
  // EXPECT_EQ(response_message[CONTENT_LEN],
  // std::to_string((strlen(TEST_CONTENT))));
}
