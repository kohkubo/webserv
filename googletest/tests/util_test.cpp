#include "gtest/gtest.h"

#include <fstream>
#include <limits.h>

#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

#define TEST_FILE          "tdata/test.txt"
#define TEST_CONTENT       "test"
#define EMPTY_FILE         "tdata/empty.txt"
#define NO_PERMISSION_FILE "tdata/no_permission.txt"
#define NO_SUCH_FILE       "no such file"

TEST(util_test, test_has_suffix) {
  std::string str    = "abcdefg";
  std::string suffix = "defg";
  EXPECT_TRUE(has_suffix(str, suffix));

  str    = "abcdefg";
  suffix = "def";
  EXPECT_FALSE(has_suffix(str, suffix));

  str    = "abcdefg";
  suffix = "abcdefg";
  EXPECT_TRUE(has_suffix(str, suffix));

  str    = "abcdefg";
  suffix = "abcdefg";
  EXPECT_TRUE(has_suffix(str, suffix));

  str    = "abcdefg";
  suffix = "abcdefgh";
  EXPECT_FALSE(has_suffix(str, suffix));
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

TEST(util_test, test_read_file_tostring) {
  std::cout << "test_read_file_tostring" << std::endl;
  EXPECT_EQ(read_file_tostring(TEST_FILE), TEST_CONTENT);
  EXPECT_EQ(read_file_tostring(EMPTY_FILE), "");
  EXPECT_EQ(read_file_tostring(NO_SUCH_FILE), ""); // TODO: エラーを拾う
  //ファイルに読み込み権限がないとNO_SUCH_FILEと同じ挙動です。
}

TEST(util_test, test_is_file_exists) {
  EXPECT_TRUE(is_file_exists(TEST_FILE));
  EXPECT_TRUE(is_file_exists(EMPTY_FILE));
  EXPECT_FALSE(is_file_exists(NO_SUCH_FILE));
  //ファイルに読み込み権限がないとtrueが返ります。
}

#define TEST_DIR "tdata/test_dir"
#define EMPTY_DIR "tdata/empty_dir"

TEST(util_test, is_dir) {
  EXPECT_TRUE(is_dir(TEST_DIR));
  EXPECT_TRUE(is_dir(EMPTY_DIR));
  EXPECT_FALSE(is_dir(TEST_FILE));
  EXPECT_FALSE(is_dir(EMPTY_FILE));
  EXPECT_FALSE(is_dir(NO_SUCH_FILE));
}

TEST(util_test, is_minus_depth_test) {
  EXPECT_TRUE(is_minus_depth("../fuga.html"));
  EXPECT_TRUE(is_minus_depth("/../fuga.html"));
  EXPECT_TRUE(is_minus_depth("hoge/../../fuga.html"));
  EXPECT_TRUE(is_minus_depth("/hoge/../../fuga.html"));
  EXPECT_FALSE(is_minus_depth("hoge/../fuga.html"));
  EXPECT_FALSE(is_minus_depth("/hoge/../fuga.html"));
  EXPECT_FALSE(is_minus_depth("hoge/../hoge/../fuga.html"));
  EXPECT_FALSE(is_minus_depth("/hoge/../hoge/../fuga.html"));
  EXPECT_TRUE(is_minus_depth("hoge/../hoge/../../fuga.html"));
  EXPECT_TRUE(is_minus_depth("/hoge/../hoge/../../fuga.html"));
}
