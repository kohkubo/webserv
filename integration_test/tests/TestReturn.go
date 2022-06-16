package tests

import (
	"integration_test/httptest"
	"net/http"
)

// テストの用意
var testReturn = testCatergory{
	categoryName: "Return",
	config:       "integration_test/conf/return.conf",
	testCases: []testCase{
		{
			caseName: "return 301",
			test: func() bool {
				port := "50002"
				expectStatusCode := 301
				expectBody := []byte{}

				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
						"Location":       {"http://location:50001/"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
