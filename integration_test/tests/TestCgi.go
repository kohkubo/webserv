package tests

import (
	"integration_test/httptest"
	"net/http"
)

var testCgi = testCatergory{
	categoryName: "cgi",
	config:       "integration_test/conf/cgi.conf",
	testCases: []testCase{
		{
			caseName: "5000_cgi_get_normal",
			test: func() bool {
				expectBody := []byte(
					"name= taro\n" +
						"blood= A\n" +
						"text= string\n",
				)

				expectStatusCode := 200
				port := "50000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /cgi_test.py?name=taro&blood=A&text=string HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
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
