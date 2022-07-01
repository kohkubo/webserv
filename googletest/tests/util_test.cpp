#include "gtest/gtest.h"

#include <fstream>
#include <limits.h>

#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

#define TEST_FILE          "html/test.txt"
#define TEST_CONTENT       "test"
#define EMPTY_FILE         "html/empty.txt"
#define NO_PERMISSION_FILE "html/no_permission.txt"
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

static void read_file_to_str_test(const std::string &path, bool is_err,
                                  const std::string &str) {
  Result<std::string> result = read_file_to_str(path);
  EXPECT_EQ(result.is_err_, is_err);
  EXPECT_EQ(result.object_, str);
}

TEST(util_test, test_read_file_to_str) {
  read_file_to_str_test(TEST_FILE, false, TEST_CONTENT);
  read_file_to_str_test(EMPTY_FILE, false, "");
  read_file_to_str_test(NO_SUCH_FILE, true, "");
}

TEST(util_test, test_is_file_exists) {
  EXPECT_TRUE(is_file_exists(TEST_FILE));
  EXPECT_TRUE(is_file_exists(EMPTY_FILE));
  EXPECT_FALSE(is_file_exists(NO_SUCH_FILE));
  //ファイルに読み込み権限がないとtrueが返ります。
}

#define TEST_DIR  "html/test_dir"
#define EMPTY_DIR "html/empty_dir"

TEST(util_test, is_dir_exists) {
  EXPECT_TRUE(is_dir_exists(TEST_DIR));
  EXPECT_TRUE(is_dir_exists(EMPTY_DIR));
  EXPECT_FALSE(is_dir_exists(TEST_FILE));
  EXPECT_FALSE(is_dir_exists(EMPTY_FILE));
  EXPECT_FALSE(is_dir_exists(NO_SUCH_FILE));
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

TEST(util_test, test_hexstr_to_size) {
  EXPECT_EQ(hexstr_to_size("0").object_, (std::size_t)0);
  EXPECT_EQ(hexstr_to_size("1").object_, (std::size_t)1);
  EXPECT_EQ(hexstr_to_size("b").object_, (std::size_t)11);
  EXPECT_EQ(hexstr_to_size("B").object_, (std::size_t)11);
  EXPECT_EQ(hexstr_to_size("f").object_, (std::size_t)15);
  EXPECT_EQ(hexstr_to_size("F").object_, (std::size_t)15);
  EXPECT_EQ(hexstr_to_size("20").object_, (std::size_t)32);
  EXPECT_EQ(hexstr_to_size("ff").object_, (std::size_t)255);
  EXPECT_EQ(hexstr_to_size("FF").object_, (std::size_t)255);
  EXPECT_EQ(hexstr_to_size("ffffffffffffffff").object_, SIZE_MAX);

  EXPECT_EQ(hexstr_to_size("0020").object_, (std::size_t)32);
  EXPECT_EQ(
      hexstr_to_size("0000000000000000000000000000000000000000020").object_,
      (std::size_t)32);

  EXPECT_TRUE(hexstr_to_size("0020h").is_err_);
  EXPECT_FALSE(hexstr_to_size("abcdef").is_err_);
  EXPECT_TRUE(hexstr_to_size("abcdefg").is_err_);
  EXPECT_FALSE(hexstr_to_size("1234567890").is_err_);
  EXPECT_TRUE(hexstr_to_size("1234567890u").is_err_);
}

TEST(util_test, test_string_to_size) {
  EXPECT_EQ(string_to_size("0").object_, (std::size_t)0);
  EXPECT_EQ(string_to_size("1").object_, (std::size_t)1);
  EXPECT_EQ(string_to_size("10").object_, (std::size_t)10);
  EXPECT_EQ(string_to_size("999").object_, (std::size_t)999);
  EXPECT_EQ(string_to_size("18446744073709551615").object_, SIZE_MAX);

  EXPECT_EQ(string_to_size("0020").object_, (std::size_t)20);
  EXPECT_EQ(
      string_to_size("0000000000000000000000000000000000000000020").object_,
      (std::size_t)20);

  EXPECT_TRUE(string_to_size("0020h").is_err_);
  EXPECT_TRUE(string_to_size("abc").is_err_);
  EXPECT_TRUE(string_to_size("-20").is_err_);
}
