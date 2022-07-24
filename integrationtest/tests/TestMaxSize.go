package tests

import (
	"integrationtest/httpresp"
	"integrationtest/httptest"
	"net/http"
)

// テストの用意
var testMaxSize = testCatergory{
	categoryName: "GET",
	config:       "integrationtest/conf/maxsize.conf",
	testCases: []testCase{
		{
			caseName: "client_body_max_size error",
			test: func() bool {
				Body := []byte(
					"name= taro\n" +
						"blood= A\n" +
						"text= string\n",
				)
				expectStatusCode := 413
				expectBody := httpresp.ErrorBody(expectStatusCode)
				port := "50000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "POST /html/cgi_test.py HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Content-Length: " + lenStr(Body) + " \r\n" +
						"\r\n" +
						string(Body),
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
