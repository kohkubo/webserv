#include "gtest/gtest.h"
#include "message.hpp"

#define TEST_FILE      "../googletest/tdata/test.txt"
#define TEST_CONTENT   "test"
#define EMPTY_FILE     "../googletest/tdata/empty.txt"
#define NO_PERMISSION_FILE  "../googletest/tdata/no_permission.txt"
#define NO_SUCH_FILE   "no such file"

TEST(get_test, target_file_exist) {
	message_type request;
	message_type response;

	request[URL] = TEST_FILE;
	get(request, response);
	EXPECT_EQ(response[STATUS], STATUS_OK);
	EXPECT_EQ(response[PHRASE], PHRASE_STATUS_OK);
	EXPECT_EQ(response[BODY], TEST_CONTENT);
	EXPECT_EQ(response[CONTENT_LEN], std::to_string((strlen(TEST_CONTENT))));
}

// 今はget()の中でNOT_FOUNDページを挿入するようになっているため
// テスト側からNOT_FOUNDページの中身について確認するのが難しいのでボディのテスト保留
TEST(get_test, target_file_not_exist) {
	message_type request;
	message_type response;

	request[URL] = NO_SUCH_FILE;
	get(request, response);
	EXPECT_EQ(response[STATUS], STATUS_NOTFOUND);
	EXPECT_EQ(response[PHRASE], PHRASE_STATUS_NOTFOUND);
	//EXPECT_EQ(response[BODY], TEST_CONTENT);
	//EXPECT_EQ(response[CONTENT_LEN], std::to_string((strlen(TEST_CONTENT))));
}
