package tests

import (
	"fmt"
	"integration_test/exe"
	"integration_test/response"
	"integration_test/tester"
	"net/http"
	"os"
)

// テストの用意
var testGET = TestCatergory{
	Name:   "GET",
	Config: "integration_test/conf/test.conf", //configをここで用意した方がわかりやすいかと
	TestCases: []TestCase{
		{
			Name: "GET / ",
			Test: func() (bool, error) {
				ExpectBody, err := exe.FileToBytes("../html/index.html")
				if err != nil {
					return false, fmt.Errorf("failt to get bytes from file")
				}
				clientA, err := tester.NewClient(&tester.Client{
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
					ExpectBody:       ExpectBody,
				})
				if err != nil {
					return false, err
				}
				return clientA.Test()
			},
		},
		{
			Name: "GET /dir1/index2.html ",
			Test: func() (bool, error) {
				ExpectBody, err := exe.FileToBytes("../html/dir1/index2.html")
				if err != nil {
					return false, fmt.Errorf("failt to get bytes from file")
				}
				clientA, err := tester.NewClient(&tester.Client{
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
					ExpectBody:       ExpectBody,
				})
				if err != nil {
					return false, err
				}
				return clientA.Test()
			},
		},
		{
			Name: "GET /no_such_file_404",
			Test: func() (bool, error) {
				clientA, err := tester.NewClient(&tester.Client{
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
				if err != nil {
					return false, err
				}
				return clientA.Test()
			},
		},

		// TODO: ファイル直接指定の場合のアクセス権限test

		{
			Name: "index解決後のアクセス権限確認test",
			Test: func() (bool, error) {
				clientA, err := tester.NewClient(&tester.Client{
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
				if err != nil {
					return false, err
				}
				os.Chmod("../html/index.html", 000)
				ok, err := clientA.Test()
				os.Chmod("../html/index.html", 0755)
				return ok, err
			},
		},
	},
}
