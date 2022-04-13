#include "gtest/gtest.h"
#include "util.hpp"
#include "limits.h"

TEST(util_test, test_is_match_suffix_string){
    std::string str = "abcdefg";
    std::string suffix = "defg";
    EXPECT_TRUE(is_match_suffix_string(str, suffix));

    str = "abcdefg";
    suffix = "def";
    EXPECT_FALSE(is_match_suffix_string(str, suffix));

    str = "abcdefg";
    suffix = "abcdefg";
    EXPECT_TRUE(is_match_suffix_string(str, suffix));

    str = "abcdefg";
    suffix = "abcdefg";
    EXPECT_TRUE(is_match_suffix_string(str, suffix));

    str = "abcdefg";
    suffix = "abcdefgh";
    EXPECT_FALSE(is_match_suffix_string(str, suffix));
}

TEST(util_test, test_tostring){
    std::size_t val = 0;
    std::string expect = "0";
    EXPECT_EQ(tostring(val), expect);

    val = 1;
    expect = "1";
    EXPECT_EQ(tostring(val), expect);

    val = 42;
    expect = "42";
    EXPECT_EQ(tostring(val), expect);


    val = INT_MAX;
    expect = std::to_string(val);
    EXPECT_EQ(tostring(val), expect);

    val = static_cast<size_t>(INT_MAX) + 1;
    expect = std::to_string(val);
    EXPECT_EQ(tostring(val), expect);

    val = SIZE_MAX;
    expect = std::to_string(val);
    EXPECT_EQ(tostring(val), expect);
}

TEST(util_test, test_read_file_tostring){
    const char *path = "../googletest/tdata/test_read_file_tostring.txt";
    std::string expect = "test";
    EXPECT_EQ(read_file_tostring(path), expect);

    path = "no_such_file";
    expect = "";
    EXPECT_EQ(read_file_tostring(path), expect);
}

TEST(util_test, test_is_file_exists){
    const char *path = "../googletest/tdata/test.txt";
    std::string expect = "test";
    EXPECT_EQ(read_file_tostring(path), expect);

    path = "no_such_file";
    expect = "";
    EXPECT_EQ(read_file_tostring(path), expect);
}
