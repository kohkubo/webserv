package tests

import (
	"integration_test/httpresp"
	"integration_test/httptest"
	"net/http"
	"strings"
)

var testBadRequest = testCatergory{
	categoryName: "badrequest",
	config:       "integration_test/conf/webserv.conf",
	testCases: []testCase{
		{
			caseName: "too long request line",
			test: func() bool {

				expectStatusCode := 400
				expectBody := httpresp.ErrorBody(expectStatusCode)

				longline := strings.Repeat("a", 8192)
				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port:             port,
					Request:          longline,
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

		{
			caseName: "too long header",
			test: func() bool {

				longline := strings.Repeat("a", 8192)
				port := "55000"
				expectStatusCode := 400
				expectBody := httpresp.ErrorBody(expectStatusCode)

				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						longline,
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

		{
			caseName: "too long content length",
			test: func() bool {

				expectStatusCode := 413
				expectBody := httpresp.ErrorBody(expectStatusCode)

				longline := strings.Repeat("a", 1025)
				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Content-Length: 1025\r\n" +
						"\r\n" +
						longline,
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

		{
			caseName: "too long chunk size line",
			test: func() bool {

				expectStatusCode := 400
				expectBody := httpresp.ErrorBody(expectStatusCode)

				longline := strings.Repeat("a", 8192)
				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Transfer-Encoding: chunked\r\n" +
						"\r\n" +
						longline,
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

		{
			caseName: "too long chunked body",
			test: func() bool {

				expectStatusCode := 413
				expectBody := httpresp.ErrorBody(expectStatusCode)

				longline := strings.Repeat("a", 1025)
				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Transfer-Encoding: chunked\r\n" +
						"\r\n" +
						"401\r\n" +
						longline +
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

		{
			caseName: "invalid chunk size",
			test: func() bool {

				expectStatusCode := 400
				expectBody := httpresp.ErrorBody(expectStatusCode)

				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Transfer-Encoding: chunked\r\n" +
						"\r\n" +
						"4\r\n" +
						"Mozilla \r\n" +
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
