package tests

import (
	"integration_test/httpresp"
	"integration_test/httptest"
	"net/http"
	"runtime"
)

// TODO: autoindex
var testAutoindex = testCatergory{
	categoryName: "autoindex",
	config:       "integration_test/conf/autoindex.conf",
	testCases: []testCase{
		{
			// 環境によってdirectoryのlistされる順番が違うみたいなのでレスポンスボディ自体を確認するのは保留
			caseName: "simple",
			test: func() bool {
				port := "50001"
				expectStatusCode := 200
				var dirList string
				switch runtime.GOOS {
				case "darwin":
					dirList = "        <li><a href=\"../\">../ </a></li>\n" +
						"        <li><a href=\"dir2/\">dir2/ </a></li>\n" +
						"        <li><a href=\"test.html\">test.html </a></li>\n" +
						"        <li><a href=\"dir1/\">dir1/ </a></li>\n"
				case "linux":
					dirList = "        <li><a href=\"dir1/\">dir1/ </a></li>\n" +
						"        <li><a href=\"test.html\">test.html </a></li>\n" +
						"        <li><a href=\"dir2/\">dir2/ </a></li>\n" +
						"        <li><a href=\"../\">../ </a></li>\n"
				}
				expectBody := []byte(
					"<!DOCTYPE html>\n" +
						"<html>\n" +
						"   <head>\n" +
						"      <title>Index of /autoindex/</title>\n" +
						"   </head>\n" +
						"   <body>\n" +
						"      <h1>Index of /autoindex/</h1>\n" +
						"      <ul style=\"list-style:none\">\n" +
						dirList +
						"    </ul>\n" +
						"   </body>\n" +
						"</html>")
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /autoindex/ HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "forbidden",
			test: func() bool {
				port := "50001"
				expectStatusCode := 403
				expectBody := httpresp.ErrorBody(403)
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /autoindex/dir2/ HTTP/1.1\r\n" +
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
			caseName: "index_priority",
			test: func() bool {
				port := "50001"
				expectStatusCode := 200
				expectBody := []byte("in test_autoindex/dir1")

				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /autoindex/dir1/ HTTP/1.1\r\n" +
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
	},
}
