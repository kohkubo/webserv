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
					Request: "POST / HTTP/1.1\r\n" +
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
					Request: "POST / HTTP/1.1\r\n" +
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
					Request: "POST / HTTP/1.1\r\n" +
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
			caseName: "invalid content length(minus)",
			test: func() bool {

				expectStatusCode := 400
				expectBody := httpresp.ErrorBody(expectStatusCode)

				longline := strings.Repeat("a", 1025)
				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "POST / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Content-Length: -1000\r\n" +
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
			caseName: "invalid chunk size",
			test: func() bool {

				expectStatusCode := 400
				expectBody := httpresp.ErrorBody(expectStatusCode)

				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "POST / HTTP/1.1\r\n" +
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

		{
			caseName: "chunk size is not hexdig",
			test: func() bool {

				expectStatusCode := 400
				expectBody := httpresp.ErrorBody(expectStatusCode)

				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "POST / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Transfer-Encoding: chunked\r\n" +
						"\r\n" +
						"4r\r\n",
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
			caseName: "get with body",
			test: func() bool {

				expectStatusCode := 400
				expectBody := httpresp.ErrorBody(expectStatusCode)

				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Content-Length: 0\r\n" +
						"\r\n" +
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
			caseName: "delete with body",
			test: func() bool {

				expectStatusCode := 400
				expectBody := httpresp.ErrorBody(expectStatusCode)

				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "DELETE / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Content-Length: 0\r\n" +
						"\r\n" +
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
			caseName: "post with no body",
			test: func() bool {

				expectStatusCode := 400
				expectBody := httpresp.ErrorBody(expectStatusCode)

				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "POST / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
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
			caseName: "content-length and chunked",
			test: func() bool {

				expectStatusCode := 400
				expectBody := httpresp.ErrorBody(expectStatusCode)

				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Content-Length: 1000\r\n" +
						"Transfer-Encoding: chunked\r\n" +
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
			caseName: "Transfer-Encoding but last member is not chunked",
			test: func() bool {

				expectStatusCode := 400
				expectBody := httpresp.ErrorBody(expectStatusCode)

				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "POST / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Transfer-Encoding: chunked, gzip\r\n" +
						"\r\n" +
						"4\r\n" +
						"test\r\n" +
						"0\r\n" +
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
			caseName: "transfer-encoding not implemented",
			test: func() bool {

				expectStatusCode := 501
				expectBody := httpresp.ErrorBody(expectStatusCode)

				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "POST / HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Transfer-Encoding: gzip, chunked\r\n" +
						"\r\n" +
						"4\r\n" +
						"test\r\n" +
						"0\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
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
