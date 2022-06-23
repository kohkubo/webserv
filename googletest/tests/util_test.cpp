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

TEST(util_test, test_hexstr_to_size) {
  EXPECT_EQ(hexstr_to_size("0"), (std::size_t)0);
  EXPECT_EQ(hexstr_to_size("1"), (std::size_t)1);
  EXPECT_EQ(hexstr_to_size("b"), (std::size_t)11);
  EXPECT_EQ(hexstr_to_size("f"), (std::size_t)15);
  EXPECT_EQ(hexstr_to_size("20"), (std::size_t)32);
  EXPECT_EQ(hexstr_to_size("ff"), (std::size_t)255);
  EXPECT_EQ(hexstr_to_size("ffffffffffffffff"), SIZE_MAX);
}
