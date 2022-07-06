#include "gtest/gtest.h"

#include "utils/convert_code.hpp"

TEST(convert_code_test, encode) {
  const std::string src     = "abc%def";
  const std::string encoded = "abc%25def";

  EXPECT_EQ(percent_encode(src), encoded);
}
