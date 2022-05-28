#include "gtest/gtest.h"

#include <string>

#include "config/Config.hpp"
#include "event/Transaction.hpp"
#include "http/request/RequestInfo.hpp"

TEST(request_chunked_test, chunked_body) {
  std::string chunked_request = "POST / HTTP/1.1\r\n"
                                "Host: hogehoge.com\r\n"
                                "Content-Type: text/plain\r\n"
                                "Transfer-Encoding: chunked\r\n"
                                "\r\n"
                                "7\r\n"
                                "Mozilla\r\n"
                                "9\r\n"
                                "Developer\r\n"
                                "7\r\n"
                                "Network\r\n"
                                "0\r\n"
                                "\r\n";

  Transaction t(-1);

  t.handle_request(chunked_request);
  const RequestInfo &info = t.get_request_info();

  EXPECT_EQ(info.is_chunked_, true);
  EXPECT_EQ(info.values_[0], "MozillaDeveloperNetwork");
}