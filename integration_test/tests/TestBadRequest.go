package tests

import (
	"integration_test/tester"
	"net/http"
	"strings"
)

var testBadRequest = testCatergory{
	categoryName: "badrequest",
	config:       "integration_test/conf/webserv.conf",
	testCases: []testCase{
		{
			caseName: "too long header",
			test: func() bool {
				longline := strings.Repeat("a", 8192)
				clientA := tester.InitClient(&tester.Client{
					Port: "5500",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						longline,
					},
					ExpectStatusCode: http.StatusBadRequest,
					ExpectHeader:     nil,
					ExpectBody:       nil,
				})
				return clientA.DoAndCheck()
			},
		},

		{
			caseName: "too long content length",
			test: func() bool {
				longline := strings.Repeat("a", 1025)
				clientA := tester.InitClient(&tester.Client{
					Port: "5500",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						"Host: localhost:5500\r\n",
						"Content-Length: 1025\r\n",
						"\r\n",
						longline,
					},
					ExpectStatusCode: http.StatusBadRequest,
					ExpectHeader:     nil,
					ExpectBody:       nil,
				})
				return clientA.DoAndCheck()
			},
		},

		{
			caseName: "too long chunked body",
			test: func() bool {
				longline := strings.Repeat("a", 1025)
				clientA := tester.InitClient(&tester.Client{
					Port: "5500",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						"Host: localhost:5500\r\n",
						"Transfer-Encoding: chunked\r\n",
						"\r\n",
						"401\r\n",
						longline,
						"\r\n",
					},
					ExpectStatusCode: http.StatusBadRequest,
					ExpectHeader:     nil,
					ExpectBody:       nil,
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
