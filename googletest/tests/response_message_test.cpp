#include "gtest/gtest.h"
#include "http/response.hpp"

TEST(response_message_test, simple_message) {
	message_type response;
  	response[VERSION] = VERSION_HTTP;
    response[STATUS] = STATUS_OK;
    response[PHRASE] = PHRASE_STATUS_OK;
	response[CONTENT_LEN] = "4";
	response[CONTENT_TYPE] = TEXT_HTML;
	response[CONNECTION]   = CONNECTION_CLOSE;
	response[BODY]   = "test";

	// フィールドの順番はfield_message()による
	std::string expect = 	"HTTP/1.1 200 OK\r\n"
							"Content-Length: 4\r\n"
							"Content-Type: text/html\r\n"
							"Connection: close\r\n"
							"\r\n"
							"test";

	EXPECT_EQ(response_message(response), expect);
}

// response_message()がエラーを出すようになったらそれもテスト
