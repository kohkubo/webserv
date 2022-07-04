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
    EXPECT_EQ(path.str(), "hogehoge/hoge");
  }
  {
    Path path("hogehoge/../hoge");
    EXPECT_EQ(path.str(), "hogehoge/../hoge");
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

TEST(path_class_test, test_has_suffix) {
  EXPECT_TRUE(Path("hoge/test/").has_suffix("/"));
  EXPECT_FALSE(Path("hoge/test").has_suffix("/"));
  EXPECT_TRUE(Path("hoge/test.py").has_suffix(".py"));
  EXPECT_FALSE(Path("hoge/test").has_suffix(".py"));
}

TEST(path_class_test, test_remove_file) {
  system("touch test_delete_target");
  Path path("test_delete_target");
  EXPECT_TRUE(path.is_file_exists());
  EXPECT_TRUE(path.remove_file());
  EXPECT_FALSE(path.is_file_exists());
}

static void read_file_to_str_test(const std::string &file_path, bool is_err,
                                  const std::string &str) {
  Path                path(file_path);
  Result<std::string> result = path.read_file_to_str();
  EXPECT_EQ(result.is_err_, is_err);
  EXPECT_EQ(result.object_, str);
}

TEST(util_test, test_read_file_to_str) {
  read_file_to_str_test(TEST_FILE, false, TEST_CONTENT);
  read_file_to_str_test(EMPTY_FILE, false, "");
  read_file_to_str_test(NO_SUCH_FILE, true, "");
}
