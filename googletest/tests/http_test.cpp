#include "gtest/gtest.h"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "config/Config.hpp"
#include "http/const/const_html_filename.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/ResponseClass.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

TEST(http_test, create_response_info_get_403) {
  Config config = Config();
  config.root_  = "../html";
  config.index_ = "index.html";
  RequestInfo request_info;
  request_info.method_  = GET;
  request_info.target_  = "/000.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  // TODO: bodyがこっちからとれない
  system("chmod 000 ../html/000.html");
  ResponseClass response_class(config, request_info);
  system("chmod 644 ../html/000.html");
  EXPECT_EQ(response_class.get_response_string(), "\
HTTP/1.1 403 Forbidden\r\n\
Host: \r\n\
Content-Length: 0\r\n\
Content-Type: text/html\r\n\
Connection: close\r\n\
\r\n\
");
}

TEST(http_test, create_response_info_delete_normal) {
  std::string expect = "HTTP/1.1 204 No Content\r\n"
                       "Connection: close\r\n\r\n";
  Config      config = Config();
  config.root_       = "../html";
  config.index_      = "index.html";
  RequestInfo request_info;
  request_info.method_  = DELETE;
  request_info.target_  = "/delete_target.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  system("touch ../html/delete_target.html");

  // TODO: テストケースこれでOK?
  ResponseClass response_class(config, request_info);
  EXPECT_EQ(response_class.get_response_string(), "\
HTTP/1.1 204 No Content\r\n\
Host: \r\n\
Content-Length: \r\n\
Content-Type: \r\n\
Connection: close\r\n\
\r\n\
");
}

TEST(http_test, create_response_info_delete_404) {
  Config config = Config();
  config.root_  = "../html";
  config.index_ = "index.html";
  RequestInfo request_info;
  request_info.method_  = DELETE;
  request_info.target_  = "/delete_target.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  // TODO: テストケースこれでOK?
  ResponseClass response_class(config, request_info);
  EXPECT_EQ(response_class.get_response_string(), "\
HTTP/1.1 404 Not Found\r\n\
Host: \r\n\
Content-Length: 0\r\n\
Content-Type: text/html\r\n\
Connection: close\r\n\
\r\n\
");
}

TEST(http_test, create_response_info_delete_403) {
  Config config = Config();
  config.root_  = "../html";
  config.index_ = "index.html";
  RequestInfo request_info;
  request_info.method_  = DELETE;
  request_info.target_  = "/000.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  system("chmod 000 ../html/000.html");

  ResponseClass response_class(config, request_info);

  system("chmod 644 ../html/000.html");

  EXPECT_EQ(response_class.get_response_string(), "\
HTTP/1.1 403 Forbidden\r\n\
Host: \r\n\
Content-Length: 0\r\n\
Content-Type: text/html\r\n\
Connection: close\r\n\
\r\n\
");
}

TEST(http_test, create_response_info_delete_400) {
  Config config = Config();
  config.root_  = "../html";
  config.index_ = "index.html";
  RequestInfo request_info;
  request_info.method_         = DELETE;
  request_info.target_         = "/hogehoge.html";
  request_info.version_        = "HTTP/1.1";
  request_info.host_           = "localhost";
  request_info.content_length_ = 8;

  ResponseClass response_class(config, request_info);

  EXPECT_EQ(response_class.get_response_string(), "\
HTTP/1.1 400 Bad Request\r\n\
Host: \r\n\
Content-Length: 0\r\n\
Content-Type: text/html\r\n\
Connection: close\r\n\
\r\n\
");
}
