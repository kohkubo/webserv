#include "gtest/gtest.h"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "config/Config.hpp"
#include "http/const/const_header_field_values.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/Response.hpp"
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
  request_info.uri_     = "/";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  Response response(server_config, request_info);
  EXPECT_EQ(response.get_response_string(), expect);
}

TEST(http_test, create_response_info_get_403) {
  Config config = Config();
  config.root_  = "../html";
  config.index_ = "index.html";
  RequestInfo request_info;
  request_info.method_  = GET;
  request_info.uri_     = "/000.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  system("chmod 000 ../html/000.html");
  Response response(config, request_info);
  system("chmod 644 ../html/000.html");
  EXPECT_EQ(response.get_response_string(), "\
HTTP/1.1 403 Forbidden\r\n\
Content-Length: 145\r\n\
Content-Type: text/html\r\n\
Connection: close\r\n\
\r\n\
<!DOCTYPE html>\n\
<html>\n\
    <head>\n\
        <title>403</title>\n\
    </head>\n\
    <body>\n\
<h2>403 Forbidden</h2>\n\
default error page\n\
    </body>\n\
</html>\
");
}

TEST(http_test, create_response_info_get_403_config_error_pages) {
  Config config = Config();
  config.root_  = "../html";
  config.index_ = "index.html";

  config.error_pages_[403] = "../html/forbidden.html";
  RequestInfo request_info;
  request_info.method_  = GET;
  request_info.uri_     = "/000.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  system("chmod 000 ../html/000.html");
  Response response(config, request_info);
  system("chmod 644 ../html/000.html");
  EXPECT_EQ(response.get_response_string(), "\
HTTP/1.1 403 Forbidden\r\n\
Content-Length: 9\r\n\
Content-Type: text/html\r\n\
Connection: close\r\n\
\r\n\
forbidden\
");
}

TEST(http_test, create_response_info_delete_normal) {
  std::string expected_response = "HTTP/1.1 204 No Content\r\n"
                                  "Connection: close\r\n\r\n";
  Config      config            = Config();
  config.root_                  = "../html";
  config.index_                 = "index.html";
  RequestInfo request_info;
  request_info.method_  = DELETE;
  request_info.uri_     = "/delete_target.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  system("touch ../html/delete_target.html");

  Response response(config, request_info);
  EXPECT_EQ(response.get_response_string(), expected_response);
}

TEST(http_test, create_response_info_delete_404) {
  Config config = Config();
  config.root_  = "../html";
  config.index_ = "index.html";
  RequestInfo request_info;
  request_info.method_  = DELETE;
  request_info.uri_     = "/delete_target.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  Response response(config, request_info);
  EXPECT_EQ(response.get_response_string(), "\
HTTP/1.1 404 Not Found\r\n\
Content-Length: 145\r\n\
Content-Type: text/html\r\n\
Connection: close\r\n\
\r\n\
<!DOCTYPE html>\n\
<html>\n\
    <head>\n\
        <title>404</title>\n\
    </head>\n\
    <body>\n\
<h2>404 Not Found</h2>\n\
default error page\n\
    </body>\n\
</html>\
");
}

TEST(http_test, create_response_info_delete_403) {
  Config config = Config();
  config.root_  = "../html";
  config.index_ = "index.html";
  RequestInfo request_info;
  request_info.method_  = DELETE;
  request_info.uri_     = "/000.html";
  request_info.version_ = "HTTP/1.1";
  request_info.host_    = "localhost";

  system("chmod 000 ../html/000.html");

  Response response(config, request_info);

  system("chmod 644 ../html/000.html");

  EXPECT_EQ(response.get_response_string(), "\
HTTP/1.1 403 Forbidden\r\n\
Content-Length: 145\r\n\
Content-Type: text/html\r\n\
Connection: close\r\n\
\r\n\
<!DOCTYPE html>\n\
<html>\n\
    <head>\n\
        <title>403</title>\n\
    </head>\n\
    <body>\n\
<h2>403 Forbidden</h2>\n\
default error page\n\
    </body>\n\
</html>\
");
}

TEST(http_test, create_response_info_delete_400) {
  Config config = Config();
  config.root_  = "../html";
  config.index_ = "index.html";
  RequestInfo request_info;
  request_info.method_         = DELETE;
  request_info.uri_            = "/hogehoge.html";
  request_info.version_        = "HTTP/1.1";
  request_info.host_           = "localhost";
  request_info.content_length_ = 8;

  Response response(config, request_info);

  EXPECT_EQ(response.get_response_string(), "\
HTTP/1.1 400 Bad Request\r\n\
Content-Length: 147\r\n\
Content-Type: text/html\r\n\
Connection: close\r\n\
\r\n\
<!DOCTYPE html>\n\
<html>\n\
    <head>\n\
        <title>400</title>\n\
    </head>\n\
    <body>\n\
<h2>400 Bad Request</h2>\n\
default error page\n\
    </body>\n\
</html>\
");
}
