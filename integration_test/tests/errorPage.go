package tests

import (
	"integration_test/httpresp"
	"integration_test/httptest"
	"net/http"
)

var testErrorPage = testCatergory{
	categoryName: "error_page",
	config:       "integration_test/conf/error_page.conf",
	testCases: []testCase{
		{
			caseName: "404 error",
			test: func() bool {
				port := "50000"
				expectStatusCode := 404
				expectBody := fileToBytes("html/not_found.html")
				client := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /no_such.html HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
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
				return client.DoAndCheck()
			},
		},
		{
			caseName: "404 error file not found",
			test: func() bool {
				port := "50001"
				expectStatusCode := 404
				expectBody := httpresp.ErrorBody(404)
				client := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /no_such.html HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
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
				return client.DoAndCheck()
			},
		},
	},
}
