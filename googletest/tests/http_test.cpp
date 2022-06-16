#include "gtest/gtest.h"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "config/Config.hpp"
#include "config/Location.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

TEST(http_test, create_response_info_get_403_config_error_pages) {
  RequestInfo request_info;
  request_info.method_         = "GET";
  request_info.request_target_ = "/000.html";
  request_info.version_        = "HTTP/1.1";
  request_info.host_           = "localhost";
  Config config;
  config.error_pages_[403] = "forbidden.html";
  config.locations_.push_back(Location());
  config.locations_[0].location_path_ = "/";
  config.locations_[0].root_          = "html/";
  config.locations_[0].index_         = "index.html";
  request_info.config_                = &config;
  request_info.location_              = &config.locations_[0];
  request_info.file_path_ =
      config.locations_[0].root_ + request_info.request_target_;

  std::string expected_response = "\
HTTP/1.1 403 Forbidden\r\n\
Connection: close\r\n\
Content-Length: 9\r\n\
Content-Type: text/html\r\n\
\r\n\
forbidden\
";

  system("chmod 000 html/000.html");

  EXPECT_EQ(ResponseGenerator::generate_response(request_info),
            expected_response);

  system("chmod 644 html/000.html");
}

TEST(http_test, create_response_info_delete_normal) {
  std::string expected_response = "HTTP/1.1 204 No Content\r\n"
                                  "Connection: close\r\n\r\n";
  RequestInfo request_info;
  request_info.method_         = "DELETE";
  request_info.request_target_ = "/delete_target.html";
  request_info.version_        = "HTTP/1.1";
  request_info.host_           = "localhost";
  Config config;
  config.locations_.push_back(Location());
  config.locations_[0].location_path_ = "/";
  config.locations_[0].root_          = "html/";
  config.locations_[0].index_         = "index.html";
  request_info.config_                = &config;
  request_info.location_              = &config.locations_[0];
  request_info.file_path_ =
      config.locations_[0].root_ + request_info.request_target_;

  system("touch html/delete_target.html");

  EXPECT_EQ(ResponseGenerator::generate_response(request_info),
            expected_response);
}

TEST(http_test, create_response_info_delete_404) {
  RequestInfo request_info;
  request_info.method_         = "DELETE";
  request_info.request_target_ = "/delete_target.html";
  request_info.version_        = "HTTP/1.1";
  request_info.host_           = "localhost";
  Config config;
  config.locations_.push_back(Location());
  config.locations_[0].location_path_ = "/";
  config.locations_[0].root_          = "html/";
  config.locations_[0].index_         = "index.html";
  request_info.config_                = &config;
  request_info.location_              = &config.locations_[0];
  request_info.file_path_ =
      config.locations_[0].root_ + request_info.request_target_;

  std::string expected_string = "\
HTTP/1.1 404 Not Found\r\n\
Connection: close\r\n\
Content-Length: 145\r\n\
Content-Type: text/html\r\n\
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
";

  EXPECT_EQ(ResponseGenerator::generate_response(request_info),
            expected_string);
}

TEST(http_test, create_response_info_delete_403) {
  RequestInfo request_info;
  request_info.method_         = "DELETE";
  request_info.request_target_ = "/000.html";
  request_info.version_        = "HTTP/1.1";
  request_info.host_           = "localhost";
  Config config;
  config.locations_.push_back(Location());
  config.locations_[0].location_path_ = "/";
  config.locations_[0].root_          = "html/";
  config.locations_[0].index_         = "index.html";
  request_info.config_                = &config;
  request_info.location_              = &config.locations_[0];
  request_info.file_path_ =
      config.locations_[0].root_ + request_info.request_target_;

  std::string expected_response = "\
HTTP/1.1 403 Forbidden\r\n\
Connection: close\r\n\
Content-Length: 145\r\n\
Content-Type: text/html\r\n\
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
";

  system("chmod 000 html/000.html");

  EXPECT_EQ(ResponseGenerator::generate_response(request_info),
            expected_response);

  system("chmod 644 html/000.html");
}

TEST(http_test, create_response_info_delete_400) {
  RequestInfo request_info;
  request_info.method_         = "DELETE";
  request_info.request_target_ = "/hogehoge.html";
  request_info.version_        = "HTTP/1.1";
  request_info.host_           = "localhost";
  request_info.content_length_ = 8;
  Config config;
  config.locations_.push_back(Location());
  config.locations_[0].location_path_ = "/";
  config.locations_[0].root_          = "html/";
  config.locations_[0].index_         = "index.html";
  request_info.config_                = &config;
  request_info.location_              = &config.locations_[0];
  request_info.file_path_ =
      config.locations_[0].root_ + request_info.request_target_;

  std::string expected_response = "\
HTTP/1.1 400 Bad Request\r\n\
Connection: close\r\n\
Content-Length: 147\r\n\
Content-Type: text/html\r\n\
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
";

  EXPECT_EQ(ResponseGenerator::generate_response(request_info),
            expected_response);
}
