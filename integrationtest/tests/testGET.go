package tests

import (
	"integration_test/httpresp"
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
				expectBody := fileToBytes("html/index.html")
				client := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
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
				expectBody := fileToBytes("html/dir1/index2.html")
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /dir1/index2.html HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "curl localhost:50000/no_such_file_404 -v",
			test: func() bool {
				port := "50000"
				expectStatusCode := 404
				expectBody := httpresp.ErrorBody(expectStatusCode)
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /no_such_file_404 HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "index解決後のアクセス権限確認test",
			test: func() bool {
				port := "50000"
				expectStatusCode := 500
				expectBody := httpresp.ErrorBody(expectStatusCode)
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				os.Chmod("html/index.html", 000)
				result := clientA.DoAndCheck()
				os.Chmod("html/index.html", 0755)
				return result
			},
		},
		{
			caseName: "ファイル直接指定の場合のアクセス権限test",
			test: func() bool {
				port := "50000"
				expectStatusCode := 500
				expectBody := httpresp.ErrorBody(expectStatusCode)
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /dir1/index2.html HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				os.Chmod("html/dir1/index2.html", 000)
				result := clientA.DoAndCheck()
				os.Chmod("html/dir1/index2.html", 0755)
				return result
			},
		},
		{
			caseName: "minus_depth ",
			test: func() bool {
				port := "50000"
				expectStatusCode := 403
				expectBody := httpresp.ErrorBody(expectStatusCode)
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /../ HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: http.StatusForbidden,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
