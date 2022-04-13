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

TEST(util_test, test_is_octet) {
  EXPECT_EQ(is_octet("0"), true);
  EXPECT_EQ(is_octet("10"), true);
  EXPECT_EQ(is_octet("99"), true);
  EXPECT_EQ(is_octet("100"), true);
  EXPECT_EQ(is_octet("199"), true);
  EXPECT_EQ(is_octet("200"), true);
  EXPECT_EQ(is_octet("249"), true);
  EXPECT_EQ(is_octet("250"), true);
  EXPECT_EQ(is_octet("255"), true);

  EXPECT_EQ(is_octet("-0"), false);
  EXPECT_EQ(is_octet("01"), false);
  EXPECT_EQ(is_octet("256"), false);
  EXPECT_EQ(is_octet("299"), false);
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
