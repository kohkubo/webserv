package tests

import (
	"integration_test/tester"
	"net/http"
)

var testAutoindex = testCatergory{
	name:   "autoindex",
	config: "integration_test/conf/autoindex.conf",
	testCases: []testCase{
		{
			// 環境によってdirectoryのlistされる順番が違うみたいなのでレスポンスボディ自体を確認するのは保留
			name: "simple",
			test: func() (bool, error) {
				clientA, err := tester.NewClient(&tester.Client{
					Port: "5001",
					ReqPayload: []string{
						"GET /autoindex/ HTTP/1.1\r\n",
						"Host: localhost:5001\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       nil,
				})
				if err != nil {
					return false, err
				}
				return clientA.DoAndCheck()
			},
		},
		{
			name: "forbidden",
			test: func() (bool, error) {
				clientA, err := tester.NewClient(&tester.Client{
					Port: "5001",
					ReqPayload: []string{
						"GET /autoindex/dir2/ HTTP/1.1\r\n",
						"Host: localhost:5001\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusForbidden,
					ExpectHeader:     nil,
					ExpectBody:       nil,
				})
				if err != nil {
					return false, err
				}
				return clientA.DoAndCheck()
			},
		},
		{

			name: "index_priority",
			test: func() (bool, error) {
				clientA, err := tester.NewClient(&tester.Client{
					Port: "5001",
					ReqPayload: []string{
						"GET /autoindex/dir1/ HTTP/1.1\r\n",
						"Host: localhost:5001\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       []byte("in test_autoindex/dir1"),
				})
				if err != nil {
					return false, err
				}
				return clientA.DoAndCheck()
			},
		},
	},
}
