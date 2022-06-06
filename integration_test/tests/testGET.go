package tests

import (
	"integration_test/response"
	"integration_test/tester"
	"net/http"
	"os"
)

// テストの用意
var testGET = testCatergory{
	name:   "GET",
	config: "integration_test/conf/test.conf",
	testCases: []testCase{
		{
			name: "GET / ",
			test: func() (bool, error) {
				clientA := tester.NewClient(&tester.Client{
					Port: "5000",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						"Host: localhost:5000\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       fileToBytes("../html/index.html"),
				})
				return clientA.DoAndCheck()
			},
		},
		{
			name: "GET /dir1/index2.html ",
			test: func() (bool, error) {
				clientA := tester.NewClient(&tester.Client{
					Port: "5000",
					ReqPayload: []string{
						"GET /dir1/index2.html HTTP/1.1\r\n",
						"Host: localhost:5000\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       fileToBytes("../html/dir1/index2.html"),
				})
				return clientA.DoAndCheck()
			},
		},
		{
			name: "GET /no_such_file_404",
			test: func() (bool, error) {
				clientA := tester.NewClient(&tester.Client{
					Port: "5000",
					ReqPayload: []string{
						"GET /no_such_file_404 HTTP/1.1\r\n",
						"Host: localhost:5000\r\n",
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
			name: "index解決後のアクセス権限確認test",
			test: func() (bool, error) {
				clientA := tester.NewClient(&tester.Client{
					Port: "5000",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						"Host: localhost:5000\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: 403,
					ExpectHeader:     nil,
					ExpectBody:       response.Content_403,
				})
				os.Chmod("../html/index.html", 000)
				ok, err := clientA.DoAndCheck()
				os.Chmod("../html/index.html", 0755)
				return ok, err
			},
		},
	},
}
