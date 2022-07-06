#include "gtest/gtest.h"

#include "utils/convert_code.hpp"

TEST(convert_code_test, encode) {
  const std::string src     = "abc%def";
  const std::string encoded = "abc%25def";

  EXPECT_EQ(percent_encode(src), encoded);

  // const std::string src     = "やすな";
  // const std::string encoded = "%E3%82%84%E3%81%99%E3%81%AA";

  // EXPECT_EQ(percent_encode(src), encoded);
}
