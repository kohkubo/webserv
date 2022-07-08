
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

bool random_read(int fd, std::string &buffer) {
  std::srand(std::time(NULL));
  std::size_t read_size = (std::rand() % 10) + 1;
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
  std::cout << "  headers.size(): " << cgi_parser.header_map_.size()
            << std::endl;
  print_map(cgi_parser.header_map_);
  std::cout << std::endl;
  std::cout << "  content_: " << cgi_parser.content_ << std::endl;
}

TEST(cgi_parse, cgi_parse_document_type) {
  int         fd = open("tdata/cgi_test_data/cgi_document.txt", O_RDONLY);
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
  EXPECT_EQ(cgi_parser.header_map_["jkldsjke"],
            "text/plain; charset=iso-8859-1");
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

static int xopen(const char *path, int flags) {
  int fd = open(path, flags);
  if (fd == -1) {
    std::cerr << "open failed: " << path << std::endl;
    exit(1);
  }
  return fd;
}

TEST(cgi_parse, cgi_parse_local_redir_reponse) {
  char tmp[256];
  getcwd(tmp, 256);
  std::cout << "Current working directory: " << tmp << std::endl;
  int         fd = xopen("tdata/cgi_test_data/cgi_local_redir.txt", O_RDONLY);
  std::string buffer;
  CgiParser   cgi_parser;
  while (random_read(fd, buffer)) {
    CgiParser::CgiState state = cgi_parser.handle_cgi(buffer);
    if (state == CgiParser::END) {
      break;
    }
  }
  // print_cgi_parser(cgi_parser);
  EXPECT_EQ(cgi_parser.cgi_location_.path_, "/tdata/cgi.py");
  EXPECT_EQ(cgi_parser.cgi_location_.query_, "");
  close(fd);
}

TEST(cgi_parse, cgi_parse_local_redir_reponse_with_query) {
  int fd =
      xopen("tdata/cgi_test_data/cgi_local_redir_with_query.txt", O_RDONLY);
  std::string buffer;
  CgiParser   cgi_parser;
  while (random_read(fd, buffer)) {
    CgiParser::CgiState state = cgi_parser.handle_cgi(buffer);
    if (state == CgiParser::END) {
      break;
    }
  }
  // print_cgi_parser(cgi_parser);
  EXPECT_EQ(cgi_parser.cgi_location_.path_, "/tdata/cgi.py");
  EXPECT_EQ(cgi_parser.cgi_location_.query_, "test=aiueo");
  close(fd);
}

TEST(cgi_parse, cgi_parse_local_redir_reponse_with_null_query) {
  int fd = xopen("tdata/cgi_test_data/cgi_local_redir_with_null_query.txt",
                 O_RDONLY);
  std::string buffer;
  CgiParser   cgi_parser;
  while (random_read(fd, buffer)) {
    CgiParser::CgiState state = cgi_parser.handle_cgi(buffer);
    if (state == CgiParser::END) {
      break;
    }
  }
  // print_cgi_parser(cgi_parser);
  EXPECT_EQ(cgi_parser.cgi_location_.path_, "/tdata/cgi.py");
  EXPECT_EQ(cgi_parser.cgi_location_.query_, "");
  close(fd);
}
