package tests

import (
	"integration_test/response"
	"integration_test/tester"
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
				clientA := tester.NewClient(tester.Client{
					Port: "50000",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						"Host: localhost:50000\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       fileToBytes("html/index.html"),
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "GET /dir1/index2.html ",
			test: func() bool {
				clientA := tester.NewClient(tester.Client{
					Port: "50000",
					ReqPayload: []string{
						"GET /dir1/index2.html HTTP/1.1\r\n",
						"Host: localhost:50000\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       fileToBytes("html/dir1/index2.html"),
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "GET /no_such_file_404",
			test: func() bool {
				clientA := tester.NewClient(tester.Client{
					Port: "50000",
					ReqPayload: []string{
						"GET /no_such_file_404 HTTP/1.1\r\n",
						"Host: localhost:50000\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusNotFound,
					ExpectHeader:     nil,
					ExpectBody:       response.Content_404,
				})
				return clientA.DoAndCheck()
			},
		},

		// TODO: ファイル直接指定の場合のアクセス権限test

		{
			caseName: "index解決後のアクセス権限確認test",
			test: func() bool {
				clientA := tester.NewClient(tester.Client{
					Port: "50000",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						"Host: localhost:50000\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: 403,
					ExpectHeader:     nil,
					ExpectBody:       response.Content_403,
				})
				os.Chmod("html/index.html", 000)
				result := clientA.DoAndCheck()
				os.Chmod("html/index.html", 0755)
				return result
			},
		},
		{
			caseName: "index解決後のアクセス権限確認test configでエラーページ指定",
			test: func() bool {
				clientA := tester.NewClient(tester.Client{
					Port: "50004",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						"Host: localhost:50004\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: 403,
					ExpectHeader:     nil,
					ExpectBody:       fileToBytes("html/forbidden.html"),
				})
				os.Chmod("html/index.html", 000)
				result := clientA.DoAndCheck()
				os.Chmod("html/index.html", 0755)
				return result
			},
		},
		{
			caseName: "minus_depth ",
			test: func() bool {
				clientA := tester.NewClient(tester.Client{
					Port: "50000",
					ReqPayload: []string{
						"GET /../ HTTP/1.1\r\n",
						"Host: localhost:50000\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusForbidden,
					ExpectHeader:     nil,
					ExpectBody:       response.Content_403,
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
