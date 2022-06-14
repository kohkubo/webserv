package tests

import (
	"integration_test/tester"
)

// テストの用意
var testReturn = testCatergory{
	categoryName: "Return",
	config:       "integration_test/conf/return.conf",
	testCases: []testCase{
		{
			caseName: "return 301",
			test: func() bool {
				clientA := tester.NewClient(tester.Client{
					Port: "50002",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						"Host: localhost:50002\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: 301,
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
