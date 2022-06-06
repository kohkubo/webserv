package tests

import (
	"integration_test/tester"
	"net/http"
)

var testServerName = testCatergory{
	categoryName: "servername",
	config:       "integration_test/conf/server_name.conf",
	testCases: []testCase{
		{
			caseName: "match_hoge",
			test: func() bool {
				clientA := tester.InitClient(&tester.Client{
					Port: "5001",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						"Host: hoge.com:5001\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       []byte("index in dir1"),
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "match_fuga",
			test: func() bool {
				clientA := tester.InitClient(&tester.Client{
					Port: "5001",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						"Host: fuga.com:5001\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       []byte("index in dir2"),
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "no_match",
			test: func() bool {
				clientA := tester.InitClient(&tester.Client{
					Port: "5001",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						"Host: no_such_host.com:5001\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       []byte("index in server_name"),
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
