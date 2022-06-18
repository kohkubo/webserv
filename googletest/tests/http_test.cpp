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

TEST(http_test, generate_response_get_normal) {
  std::string expect = "HTTP/1.1 200 OK\r\n"
                       "Connection: close\r\n"
                       "Content-Length: 127\r\n"
                       "Content-Type: text/html\r\n"
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
  Config      config;
  config.locations_.push_back(Location());
  config.locations_[0].location_path_ = "/";
  config.locations_[0].root_          = "html/";
  config.locations_[0].index_         = "index.html";
  RequestInfo request_info;
  request_info.method_         = "GET";
  request_info.request_target_ = "/";
  request_info.version_        = "HTTP/1.1";
  request_info.host_           = "localhost";

  EXPECT_EQ(ResponseGenerator::generate_response(config, request_info), expect);
}

TEST(http_test, generate_response_get_403) {
  Config config;
  config.locations_.push_back(Location());
  config.locations_[0].location_path_ = "/";
  config.locations_[0].root_          = "html/";
  config.locations_[0].index_         = "index.html";
  RequestInfo request_info;
  request_info.method_          = "GET";
  request_info.request_target_  = "/000.html";
  request_info.version_         = "HTTP/1.1";
  request_info.host_            = "localhost";

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

  EXPECT_EQ(ResponseGenerator::generate_response(config, request_info),
            expected_response);

  system("chmod 644 html/000.html");
}

TEST(http_test, generate_response_get_403_config_error_pages) {
  Config config;
  config.locations_.push_back(Location());
  config.locations_[0].location_path_ = "/";
  config.locations_[0].root_          = "html/";
  config.locations_[0].index_         = "index.html";

  config.error_pages_[403]            = "forbidden.html";
  RequestInfo request_info;
  request_info.method_          = "GET";
  request_info.request_target_  = "/000.html";
  request_info.version_         = "HTTP/1.1";
  request_info.host_            = "localhost";

  std::string expected_response = "\
HTTP/1.1 403 Forbidden\r\n\
Connection: close\r\n\
Content-Length: 9\r\n\
Content-Type: text/html\r\n\
\r\n\
forbidden\
";

  system("chmod 000 html/000.html");

  EXPECT_EQ(ResponseGenerator::generate_response(config, request_info),
            expected_response);

  system("chmod 644 html/000.html");
}

TEST(http_test, generate_response_delete_normal) {
  std::string expected_response = "HTTP/1.1 204 No Content\r\n"
                                  "Connection: close\r\n\r\n";
  Config      config;
  config.locations_.push_back(Location());
  config.locations_[0].location_path_ = "/";
  config.locations_[0].root_          = "html/";
  config.locations_[0].index_         = "index.html";
  RequestInfo request_info;
  request_info.method_         = "DELETE";
  request_info.request_target_ = "/delete_target.html";
  request_info.version_        = "HTTP/1.1";
  request_info.host_           = "localhost";

  system("touch html/delete_target.html");

  EXPECT_EQ(ResponseGenerator::generate_response(config, request_info),
            expected_response);
}

TEST(http_test, generate_response_delete_404) {
  Config config;
  config.locations_.push_back(Location());
  config.locations_[0].location_path_ = "/";
  config.locations_[0].root_          = "html/";
  config.locations_[0].index_         = "index.html";
  RequestInfo request_info;
  request_info.method_         = "DELETE";
  request_info.request_target_ = "/delete_target.html";
  request_info.version_        = "HTTP/1.1";
  request_info.host_           = "localhost";

  std::string expected_string  = "\
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

  EXPECT_EQ(ResponseGenerator::generate_response(config, request_info),
            expected_string);
}

TEST(http_test, generate_response_delete_403) {
  Config config;
  config.locations_.push_back(Location());
  config.locations_[0].location_path_ = "/";
  config.locations_[0].root_          = "html/";
  config.locations_[0].index_         = "index.html";
  RequestInfo request_info;
  request_info.method_          = "DELETE";
  request_info.request_target_  = "/000.html";
  request_info.version_         = "HTTP/1.1";
  request_info.host_            = "localhost";

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

  EXPECT_EQ(ResponseGenerator::generate_response(config, request_info),
            expected_response);

  system("chmod 644 html/000.html");
}

TEST(http_test, generate_response_delete_400) {
  Config config;
  config.locations_.push_back(Location());
  config.locations_[0].location_path_ = "/";
  config.locations_[0].root_          = "html/";
  config.locations_[0].index_         = "index.html";
  RequestInfo request_info;
  request_info.method_          = "DELETE";
  request_info.request_target_  = "/hogehoge.html";
  request_info.version_         = "HTTP/1.1";
  request_info.host_            = "localhost";
  request_info.content_length_  = 8;

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

  EXPECT_EQ(ResponseGenerator::generate_response(config, request_info),
            expected_response);
}

static void add_form(RequestInfo::formMap &fmap, const std::string name,
                     const std::string filename, const std::string content) {
  RequestInfo::Form f;
  f.content_disposition_.type_              = "form-data";
  f.content_disposition_.parameter_["name"] = name;
  if (filename != "") {
    f.content_disposition_.parameter_["filename"] = filename;
  }
  f.content_ = content;
  fmap[name] = f;
}

// TEST(http_test, generate_response_post_normal) {
//   Config config;
//   config.locations_.push_back(Location());
//   config.locations_[0].location_path_ = "/";
//   config.locations_[0].root_          = "tdata/utils_test/";
//   config.locations_[0].upload_file_   = true;
//   RequestInfo request_info;
//   request_info.method_             = "POST";
//   request_info.request_target_     = "/";
//   request_info.version_            = "HTTP/1.1";
//   request_info.host_               = "localhost";
//   request_info.content_type_.type_ = "multipart/form-data";
//   add_form(request_info.form_map_, "test0", "", "notfile");
//   add_form(request_info.form_map_, "test1", "test1.txt",
//   "test1_content\r\n"); add_form(request_info.form_map_, "test2",
//   "test2.txt", "test2_content\r\n");

//  std::string expect = "HTTP/1.1 201 Created\r\n"
//                       "Connection: close\r\n"
//                       "Content-Length: 127\r\n"
//                       "Content-Type: text/html\r\n"
//                       "\r\n"
//                       "created";

//  EXPECT_EQ(ResponseGenerator::generate_response(config, request_info),
//  expect);
//}
