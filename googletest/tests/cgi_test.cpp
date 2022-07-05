
#include "gtest/gtest.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <ctime>
#include <random>
#include <string>

#include "config/ConfGroupGen.hpp"
#include "config/Config.hpp"
#include "event/Request.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/CgiParser.hpp"
#include "utils/Result.hpp"

TEST(cgi_test, normal) {
  std::string request_buffer = "GET / HTTP/1.1\r\n"
                               "Host: 127.0.0.1:50001\r\n"
                               "User-Agent: curl/7.68.0\r\n"
                               "Connection: close\r\n"
                               "Accept: */*\r\n\r\n";

  Config      config;
  ConfigGroup config_group(config);
  Request     request;

  request.handle_request(request_buffer, config_group);
  const RequestInfo &request_info = request.request_info();

  EXPECT_EQ(request_info.request_line_.method_, "GET");
  EXPECT_EQ(request_info.request_line_.absolute_path_.str(), "/");
  EXPECT_EQ(request_info.request_line_.http_version_, "HTTP/1.1");
  EXPECT_EQ(request_info.host_, "127.0.0.1");
  EXPECT_EQ(request_info.connection_close_, true);
}

TEST(cgi_test, normal_delete) {
  std::string request_buffer = "DELETE /delete_target.tmp HTTP/1.1\r\n"
                               "Host: 127.0.0.1:50001\r\n"
                               "User-Agent: curl/7.68.0\r\n"
                               "Connection: close\r\n"
                               "Accept: */*\r\n\r\n";

  Config      config;
  ConfigGroup config_group(config);
  Request     request;

  request.handle_request(request_buffer, config_group);
  const RequestInfo &request_info = request.request_info();

  EXPECT_EQ(request_info.request_line_.method_, "DELETE");
  EXPECT_EQ(request_info.request_line_.absolute_path_.str(),
            "/delete_target.tmp");
  EXPECT_EQ(request_info.request_line_.http_version_, "HTTP/1.1");
  EXPECT_EQ(request_info.host_, "127.0.0.1");
  EXPECT_EQ(request_info.connection_close_, true);
}

TEST(cgi_test, normal_post) {
  std::string request_buffer =
      "POST /target HTTP/1.1\r\n"
      "Host: 127.0.0.1:50001\r\n"
      "User-Agent: curl/7.68.0\r\n"
      "Connection: close\r\n"
      "Accept: */*\r\n"
      "Content-Type: application/x-www-form-urlencoded\r\n"
      "Content-Length: 18\r\n\r\n";

  Config      config;
  ConfigGroup config_group(config);
  Request     request;

  request.handle_request(request_buffer, config_group);
  const RequestInfo &request_info = request.request_info();

  EXPECT_EQ(request_info.request_line_.method_, "POST");
  EXPECT_EQ(request_info.request_line_.absolute_path_.str(), "/target");
  EXPECT_EQ(request_info.request_line_.http_version_, "HTTP/1.1");
  EXPECT_EQ(request_info.host_, "127.0.0.1");
  EXPECT_EQ(request_info.content_type_, "application/x-www-form-urlencoded");
  EXPECT_EQ(request_info.connection_close_, true);
  EXPECT_EQ(request_info.content_length_, static_cast<std::size_t>(18));
}

TEST(cgi_test, query_string) {
  std::string request_buffer = "GET /hoge.cgi?name=taro&age=15 HTTP/1.1\r\n"
                               "Host: 127.0.0.1:50001\r\n"
                               "User-Agent: curl/7.68.0\r\n"
                               "Connection: close\r\n"
                               "Accept: */*\r\n\r\n";

  Config      config;
  ConfigGroup config_group(config);
  Request     request;

  request.handle_request(request_buffer, config_group);
  const RequestInfo &request_info = request.request_info();

  EXPECT_EQ(request_info.request_line_.method_, "GET");
  EXPECT_EQ(request_info.request_line_.absolute_path_.str(), "/hoge.cgi");
  EXPECT_EQ(request_info.request_line_.query_, "name=taro&age=15");
}

bool random_read(int fd, std::string &buffer) {
  std::srand(std::time(NULL));
  std::size_t read_size = (std::rand() % 10);
  char       *buf       = new char[read_size];
  int         ret       = read(fd, buf, read_size);

  if (ret == -1) {
    delete[] buf;
    return false;
  }
  if (ret == 0) {
    delete[] buf;
    return false;
  }
  buffer.append(buf, ret);
  delete[] buf;
  return true;
}

void print_map(const std::map<std::string, std::string> &map) {
  std::map<std::string, std::string>::const_iterator it;
  for (it = map.begin(); it != map.end(); ++it) {
    std::cout << "  " << it->first << ": " << it->second << std::endl;
  }
}

void print_cgi_parser(const CgiParser &cgi_parser) {
  std::cout << "CgiParser:" << std::endl;
  std::cout << "  content_type_: " << cgi_parser.content_type_ << std::endl;
  std::cout << "  content_length_: " << cgi_parser.content_length_ << std::endl;
  std::cout << "  new_line_delimiter_: " << cgi_parser.new_line_delimiter_
            << std::endl;
  std::cout << "  headers.size(): " << cgi_parser.header_map_.size()
            << std::endl;
  print_map(cgi_parser.header_map_);
  std::cout << std::endl;
  std::cout << "  content_: " << cgi_parser.content_ << std::endl;
}

TEST(cgi_test, cgi_parse) {
  int         fd = open("tdata/cgi.txt", O_RDONLY);
  std::string buffer;
  CgiParser   cgi_parser;
  while (random_read(fd, buffer)) {
    CgiParser::CgiState state = cgi_parser.handle_cgi(buffer);
    if (state == CgiParser::END) {
      break;
    }
  }
  // print_cgi_parser(cgi_parser);
  EXPECT_EQ(cgi_parser.content_type_, "text/plain");
  EXPECT_EQ(cgi_parser.content_length_, (std::size_t)378);
  EXPECT_EQ(cgi_parser.header_map_["jkldsjke"], "text/plain; charset=iso-8859-1");
  EXPECT_EQ(cgi_parser.content_, "\
CGI/1.0 test script report:\n\
\n\
argc is 0. argv is .\n\
\n\
SERVER_SOFTWARE = asdfsf\n\
SERVER_NAME = fdsfeede\n\
GATEWAY_INTERFACE = dfsdkhdjjd\n\
SERVER_PROTOCOL = djdjdjd\n\
SERVER_PORT = ddd\n\
REQUEST_METHOD = sdkgjgjh  dfsdfjkl\n\
HTTP_ACCEPT = dkk34kk5j\n\
PATH_INFO =\n\
PATH_TRANSLATED =\n\
SCRIPT_NAME =\n\
QUERY_STRING =\n\
REMOTE_HOST =\n\
REMOTE_ADDR =\n\
REMOTE_USER =\n\
AUTH_TYPE =\n\
CONTENT_TYPE =\n\
CONTENT_LENGTH =");
  close(fd);
}