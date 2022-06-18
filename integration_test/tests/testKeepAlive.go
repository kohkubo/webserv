package tests

import (
	"integration_test/httptest"
	"net/http"
)

// テストの用意
var testKeepAlive = testCatergory{
	categoryName: "KeepAlive",
	config:       "integration_test/conf/test.conf",
	testCases: []testCase{
		{
			caseName: "serial request",
			test: func() bool {
				res := true
				port := "50000"
				expectStatusCode := 200
				expectBody := fileToBytes("html/index.html")
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				clientA.Close = false
				res = res && clientA.DoAndCheck()
				clientB := httptest.NewClientWithConn(clientA.Conn, httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				res = res && clientB.DoAndCheck()
				return res
			},
		},
	},
}
