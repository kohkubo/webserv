package tests

import (
	"integration_test/tester"
	"net/http"
)

var testAutoindex = testCatergory{
	categoryName: "autoindex",
	config:       "integration_test/conf/autoindex.conf",
	testCases: []testCase{
		{
			// 環境によってdirectoryのlistされる順番が違うみたいなのでレスポンスボディ自体を確認するのは保留
			caseName: "simple",
			test: func() bool {
				clientA := tester.NewClient(tester.Client{
					Port: "50001",
					ReqPayload: []string{
						"GET /autoindex/ HTTP/1.1\r\n",
						"Host: localhost:50001\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       nil,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "forbidden",
			test: func() bool {
				clientA := tester.NewClient(tester.Client{
					Port: "50001",
					ReqPayload: []string{
						"GET /autoindex/dir2/ HTTP/1.1\r\n",
						"Host: localhost:50001\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusForbidden,
					ExpectHeader:     nil,
					ExpectBody:       nil,
				})
				return clientA.DoAndCheck()
			},
		},
		{

			caseName: "index_priority",
			test: func() bool {
				clientA := tester.NewClient(tester.Client{
					Port: "50001",
					ReqPayload: []string{
						"GET /autoindex/dir1/ HTTP/1.1\r\n",
						"Host: localhost:50001\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       []byte("in test_autoindex/dir1"),
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
