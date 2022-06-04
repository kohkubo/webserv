package tests

import (
	"integration_test/tester"
	"net/http"
	"strings"
)

var testBadRequest = testCatergory{
	Name:   "badrequest",
	Config: "integration_test/conf/webserv.conf",
	TestCases: []testCase{
		{
			Name: "too long header",
			Test: func() (bool, error) {
				longline := strings.Repeat("a", 8192)
				clientA, err := tester.NewClient(&tester.Client{
					Port: "5500",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						longline,
					},
					ExpectStatusCode: http.StatusBadRequest,
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
			Name: "too long content length",
			Test: func() (bool, error) {
				longline := strings.Repeat("a", 1025)
				clientA, err := tester.NewClient(&tester.Client{
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
				if err != nil {
					return false, err
				}
				return clientA.DoAndCheck()
			},
		},

		{
			Name: "too long chunked body",
			Test: func() (bool, error) {
				longline := strings.Repeat("a", 1025)
				clientA, err := tester.NewClient(&tester.Client{
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
				if err != nil {
					return false, err
				}
				return clientA.DoAndCheck()
			},
		},
	},
}
