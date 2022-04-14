#include "limits.h"
#include "util.hpp"
#include "gtest/gtest.h"

TEST(util_test, test_is_match_suffix_string) {
  std::string str    = "abcdefg";
  std::string suffix = "defg";
  EXPECT_TRUE(is_match_suffix_string(str, suffix));

  str    = "abcdefg";
  suffix = "def";
  EXPECT_FALSE(is_match_suffix_string(str, suffix));

  str    = "abcdefg";
  suffix = "abcdefg";
  EXPECT_TRUE(is_match_suffix_string(str, suffix));

  str    = "abcdefg";
  suffix = "abcdefg";
  EXPECT_TRUE(is_match_suffix_string(str, suffix));

  str    = "abcdefg";
  suffix = "abcdefgh";
  EXPECT_FALSE(is_match_suffix_string(str, suffix));
}

TEST(util_test, test_to_string) {
  EXPECT_EQ(to_string(0), "0");
  EXPECT_EQ(to_string(1), "1");
  EXPECT_EQ(to_string(INT_MAX), std::to_string(INT_MAX));
  EXPECT_EQ(to_string(SIZE_MAX), std::to_string(SIZE_MAX));
  // SIZE_MAX を超えると0になります。
  EXPECT_EQ(to_string(SIZE_MAX + 1), "0");
}

TEST(util_test, test_is_uint8) {
  EXPECT_EQ(is_uint8("0"), true);
  EXPECT_EQ(is_uint8("10"), true);
  EXPECT_EQ(is_uint8("99"), true);
  EXPECT_EQ(is_uint8("100"), true);
  EXPECT_EQ(is_uint8("199"), true);
  EXPECT_EQ(is_uint8("200"), true);
  EXPECT_EQ(is_uint8("249"), true);
  EXPECT_EQ(is_uint8("250"), true);
  EXPECT_EQ(is_uint8("255"), true);

  EXPECT_EQ(is_uint8("-0"), false);
  EXPECT_EQ(is_uint8("01"), false);
  EXPECT_EQ(is_uint8("256"), false);
  EXPECT_EQ(is_uint8("299"), false);
}

TEST(util_test, test_is_ip) {
  EXPECT_EQ(is_ip("0.0.0.0"), true);
  EXPECT_EQ(is_ip("255.255.255.255"), true);
  EXPECT_EQ(is_ip("192.168.0.0"), true);
  EXPECT_EQ(is_ip("192.168.0.1"), true);

  EXPECT_EQ(is_ip("256.255.255.255"), false);
  EXPECT_EQ(is_ip("255.255.255.256"), false);
  EXPECT_EQ(is_ip("00.0.0.0"), false);
  EXPECT_EQ(is_ip("9999"), false);
  EXPECT_EQ(is_ip("localhost"), false);
  EXPECT_EQ(is_ip("192.168.090.0"), false);
}

TEST(util_test, test_is_digits) {
  EXPECT_EQ(is_digits("123"), true);
  EXPECT_EQ(is_digits("9999"), true);
  EXPECT_EQ(is_digits("0000"), true);

  EXPECT_EQ(is_digits("-123"), false);
  EXPECT_EQ(is_digits("hello"), false);
}

TEST(util_test, test_sizet_to_string) {
  std::size_t val    = 0;
  std::string expect = "0";
  EXPECT_EQ(sizet_to_string(val), expect);

  val    = 1;
  expect = "1";
  EXPECT_EQ(sizet_to_string(val), expect);

  val    = 42;
  expect = "42";
  EXPECT_EQ(sizet_to_string(val), expect);

  val = INT_MAX;
  EXPECT_EQ(sizet_to_string(val), std::to_string(val));

  val = static_cast<size_t>(INT_MAX) + 1;
  EXPECT_EQ(sizet_to_string(val), std::to_string(val));

  val = SIZE_MAX;
  EXPECT_EQ(sizet_to_string(val), std::to_string(val));
}

TEST(util_test, test_read_file_to_string) {
  std::string path   = "../googletest/tdata/test.txt";
  std::string expect = "test";
  EXPECT_EQ(read_file_to_string(path), expect);

  path   = "no_such_file";
  expect = "file not found";
  EXPECT_EQ(read_file_to_string(path), expect);
}
