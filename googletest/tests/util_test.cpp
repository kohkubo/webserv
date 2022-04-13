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

TEST(util_test, test_sizet_to_string){
    std::size_t val = 0;
    std::string expect = "0";
    EXPECT_EQ(sizet_to_string(val), expect);

    val = 1;
    expect = "1";
    EXPECT_EQ(sizet_to_string(val), expect);

    val = 42;
    expect = "42";
    EXPECT_EQ(sizet_to_string(val), expect);


    val = INT_MAX;
    EXPECT_EQ(sizet_to_string(val), std::to_string(val));

    val = static_cast<size_t>(INT_MAX) + 1;
    EXPECT_EQ(sizet_to_string(val), std::to_string(val));

    val = SIZE_MAX;
    EXPECT_EQ(sizet_to_string(val), std::to_string(val));
}

TEST(util_test, test_read_file_to_string){
    std::string path = "../googletest/tdata/test.txt";
    std::string expect = "test";
    EXPECT_EQ(read_file_to_string(path), expect);

    path = "no_such_file";
    expect = "file not found";
    EXPECT_EQ(read_file_to_string(path), expect);
}
