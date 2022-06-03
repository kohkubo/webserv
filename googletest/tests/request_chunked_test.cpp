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
                                "8\r\n"
                                "Mozilla \r\n"
                                "28\r\n"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\r\n"
                                "8\r\n"
                                " Network\r\n"
                                "10\r\n"
                                "hello world\ntest\r\n"
                                "0\r\n"
                                "\r\n";

  Config      config;
  confGroup   conf_group;
  conf_group.push_back(&config);
  Transaction transaction;

  transaction.handle_request(chunked_request, conf_group);
  const RequestInfo &info = transaction.get_request_info();

  EXPECT_EQ(info.is_chunked_, true);
  EXPECT_EQ(info.env_values_[0],
            "Mozilla aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa Networkhello "
            "world\ntest");
}

TEST(request_chunked_test, chunked_body_length_exception) {
  std::string chunked_request = "POST / HTTP/1.1\r\n"
                                "Host: hogehoge.com\r\n"
                                "Content-Type: text/plain\r\n"
                                "Transfer-Encoding: chunked\r\n"
                                "\r\n"
                                "4\r\n"
                                "Mozilla \r\n"
                                "\r\n";

  Config      config;
  confGroup   conf_group;
  conf_group.push_back(&config);
  Transaction transaction;

  EXPECT_THROW(transaction.handle_request(chunked_request, conf_group),
               RequestInfo::BadRequestException);
}

TEST(request_chunked_test, chunked_body_split) {
  std::string split_request1 = "POST / HTTP/1.1\r\n"
                               "Host: hogehoge.com\r\n"
                               "Content-Type: text/plain\r\n"
                               "Transfer-Encoding: chunked\r\n"
                               "\r\n"
                               "8\r\n"
                               "Moz";
  std::string split_request2 = "illa \r\n"
                               "2";
  std::string split_request3 = "8\r\n"
                               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\r\n"
                               "8\r\n"
                               " Network\r\n"
                               "10\r\n"
                               "hello world\ntest\r\n"
                               "0\r\n"
                               "\r\n";

  Config      config;
  confGroup   conf_group;
  conf_group.push_back(&config);
  Transaction transaction;
  std::string request_buffer;
  request_buffer.append(split_request1);
  transaction.handle_request(request_buffer, conf_group);
  request_buffer.append(split_request2);
  transaction.handle_request(request_buffer, conf_group);
  request_buffer.append(split_request3);
  transaction.handle_request(request_buffer, conf_group);

  const RequestInfo &info = transaction.get_request_info();

  EXPECT_EQ(info.is_chunked_, true);
  EXPECT_EQ(info.env_values_[0],
            "Mozilla aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa Networkhello "
            "world\ntest");
}
