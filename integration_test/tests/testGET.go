package tests

import (
	"integration_test/httptest"
	"net/http"
	"os"
)

// テストの用意
var testGET = testCatergory{
	categoryName: "GET",
	config:       "integration_test/conf/test.conf",
	testCases: []testCase{
		{
			caseName: "GET / ",
			test: func() bool {
				port := "50000"
				expectStatusCode := 200
				expectBody, contentLen := bytesAndLen("../html/index.html")
				client := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
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
				return client.DoAndCheck()
			},
		},
		{
			caseName: "GET /dir1/index2.html ",
			test: func() bool {
				port := "50000"
				expectStatusCode := 200
				expectBody, contentLen := bytesAndLen("../html/dir1/index2.html")
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /dir1/index2.html HTTP/1.1\r\n" +
						"Host: localhost:50000\r\n" +
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
			caseName: "GET /no_such_file_404",
			test: func() bool {
				port := "50000"
				expectStatusCode := 404
				expectBody, contentLen := errBytesAndLen(expectStatusCode)
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /no_such_file_404 HTTP/1.1\r\n" +
						"Host: localhost:50000\r\n" +
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

		// TODO: ファイル直接指定の場合のアクセス権限test

		{
			caseName: "index解決後のアクセス権限確認test",
			test: func() bool {
				port := "50000"
				expectStatusCode := 403
				expectBody, contentLen := errBytesAndLen(expectStatusCode)
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:50000\r\n" +
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
				os.Chmod("../html/index.html", 000)
				result := clientA.DoAndCheck()
				os.Chmod("../html/index.html", 0755)
				return result
			},
		},
		{
			caseName: "minus_depth ",
			test: func() bool {
				port := "50000"
				expectStatusCode := 403
				expectBody, contentLen := errBytesAndLen(expectStatusCode)
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /../ HTTP/1.1\r\n" +
						"Host: localhost:50000\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: http.StatusForbidden,
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
