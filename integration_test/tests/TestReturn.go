package tests

import (
	"integration_test/httptest"
)

// テストの用意
var testReturn = testCatergory{
	categoryName: "Return",
	config:       "integration_test/conf/return.conf",
	testCases: []testCase{
		{
			caseName: "return 301",
			test: func() bool {

				//expectBody := fileToBytes("../html/index.html")
				//contentLen := strconv.Itoa(len(expectBody))
				clientA := httptest.NewClient(httptest.TestSource{
					Port: "50002",
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:50002\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: 301,
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
