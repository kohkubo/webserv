#include "gtest/gtest.h"
#include "http.hpp"

TEST(response_message_test, simple_message) {
	http_message response_message;
  	response_message[VERSION] = VERSION_HTTP;
    response_message[STATUS] = STATUS_OK;
    response_message[PHRASE] = PHRASE_STATUS_OK;
	response_message[CONTENT_LEN] = "4";
	response_message[CONTENT_TYPE] = TEXT_HTML;
	response_message[CONNECTION]   = CONNECTION_CLOSE;
	response_message[BODY]   = "test";

	// フィールドの順番はfield_message()による
	std::string expect = 	"HTTP/1.1 200 OK\r\n"
							"Content-Length: 4\r\n"
							"Content-Type: text/html\r\n"
							"Connection: close\r\n"
							"\r\n"
							"test";

	EXPECT_EQ(http_response_message_get(response_message), expect);
}

// response_message()がエラーを出すようになったらそれもテスト
