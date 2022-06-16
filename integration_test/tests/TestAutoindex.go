package tests

import (
	"integration_test/httptest"
	"net/http"
	"strconv"
)

// TODO: autoindex
var testAutoindex = testCatergory{
	categoryName: "autoindex",
	config:       "integration_test/conf/autoindex.conf",
	testCases: []testCase{
		//{
		//	// 環境によってdirectoryのlistされる順番が違うみたいなのでレスポンスボディ自体を確認するのは保留
		//	caseName: "simple",
		//	test: func() bool {
		//		port := "50001"
		//		expectStatusCode := 200
		//		expectBody, contentLen := bytesAndLen("html/index.html")
		//		clientA := httptest.NewClient(httptest.TestSource{
		//			Port: port,
		//			Request: "GET /autoindex/ HTTP/1.1\r\n" +
		//				"Host: localhost:" + port + "\r\n" +
		//				"User-Agent: curl/7.79.1\r\n" +
		//				`Accept: */*` + "\r\n" +
		//				"\r\n",
		//			ExpectStatusCode: expectStatusCode,
		//			ExpectHeader: http.Header{
		//				"Connection":     {"close"},
		//				"Content-Length": {contentLen},
		//				"Content-Type":   {"text/html"},
		//			},
		//			ExpectBody: expectBody,
		//		})
		//		return clientA.DoAndCheck()
		//	},
		//},
		{
			caseName: "forbidden",
			test: func() bool {
				port := "50001"
				expectStatusCode := 403
				expectBody, contentLen := errBytesAndLen(403)
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /autoindex/dir2/ HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {contentLen},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "index_priority",
			test: func() bool {
				port := "50001"
				expectStatusCode := 200
				expectBody := []byte("in test_autoindex/dir1")
				contentLen := strconv.Itoa(len(expectBody))
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /autoindex/dir1/ HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {contentLen},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
