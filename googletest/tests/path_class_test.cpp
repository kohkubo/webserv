#include "gtest/gtest.h"

#include "utils/Path.hpp"

#define TEST_FILE          "html/test.txt"
#define TEST_CONTENT       "test"
#define EMPTY_FILE         "html/empty.txt"
#define NO_PERMISSION_FILE "html/no_permission.txt"
#define NO_SUCH_FILE       "no such file"

#define TEST_DIR           "html/test_dir"
#define EMPTY_DIR          "html/empty_dir"
#define SUFFIX_DIR         "html/empty_dir/"

TEST(path_class_test, constructor) {
  {
    Path path("hogehoge/hoge");
    EXPECT_EQ(path.path(), "hogehoge/hoge");
  }
  {
    Path path("hogehoge/../hoge");
    EXPECT_EQ(path.path(), "hogehoge/../hoge");
  }
}

TEST(path_class_test, test_is_file_exists) {
  EXPECT_TRUE(Path(TEST_FILE).is_file_exists());
  EXPECT_TRUE(Path(EMPTY_FILE).is_file_exists());
  EXPECT_FALSE(Path(NO_SUCH_FILE).is_file_exists());
  //ファイルに読み込み権限がないとtrueが返ります。
}

TEST(path_class_test, is_dir_exists) {
  EXPECT_TRUE(Path(TEST_DIR).is_dir_exists());
  EXPECT_TRUE(Path(EMPTY_DIR).is_dir_exists());
  EXPECT_TRUE(Path(SUFFIX_DIR).is_dir_exists());
  EXPECT_FALSE(Path(TEST_FILE).is_dir_exists());
  EXPECT_FALSE(Path(EMPTY_FILE).is_dir_exists());
  EXPECT_FALSE(Path(NO_SUCH_FILE).is_dir_exists());
}

TEST(path_class_test, is_minus_depth_test) {
  EXPECT_TRUE(Path("../fuga.html").is_minus_depth());
  EXPECT_TRUE(Path("/../fuga.html").is_minus_depth());
  EXPECT_TRUE(Path("hoge/../../fuga.html").is_minus_depth());
  EXPECT_TRUE(Path("/hoge/../../fuga.html").is_minus_depth());
  EXPECT_FALSE(Path("hoge/../fuga.html").is_minus_depth());
  EXPECT_FALSE(Path("/hoge/../fuga.html").is_minus_depth());
  EXPECT_FALSE(Path("hoge/../hoge/../fuga.html").is_minus_depth());
  EXPECT_FALSE(Path("/hoge/../hoge/../fuga.html").is_minus_depth());
  EXPECT_TRUE(Path("hoge/../hoge/../../fuga.html").is_minus_depth());
  EXPECT_TRUE(Path("/hoge/../hoge/../../fuga.html").is_minus_depth());
}

TEST(path_class_test, is_accessible) {
  EXPECT_TRUE(Path(TEST_FILE).is_accessible(W_OK));
  EXPECT_TRUE(Path(TEST_FILE).is_accessible(R_OK));
  system("chmod 222 html/test.txt");
  EXPECT_TRUE(Path(TEST_FILE).is_accessible(W_OK));
  EXPECT_FALSE(Path(TEST_FILE).is_accessible(R_OK));
  system("chmod 644 html/test.txt");
  system("chmod 000 html/test.txt");
  EXPECT_FALSE(Path(TEST_FILE).is_accessible(W_OK));
  EXPECT_FALSE(Path(TEST_FILE).is_accessible(R_OK));
  system("chmod 644 html/test.txt");
  system("chmod 444 html/test.txt");
  EXPECT_FALSE(Path(TEST_FILE).is_accessible(W_OK));
  EXPECT_TRUE(Path(TEST_FILE).is_accessible(R_OK));
  system("chmod 644 html/test.txt");
}
