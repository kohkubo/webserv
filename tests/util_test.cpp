#include "gtest/gtest.h"
#include "util.hpp"

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
