package tests

import (
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

				expectBody, contentLen := bytesAndLen("../html/index.html")

				longline := strings.Repeat("a", 8192)
				clientA := httptest.NewClient(httptest.TestSource{
					Port:             "55000",
					Request:          longline,
					ExpectStatusCode: 400,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {contentLen},
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

				expectBody, contentLen := bytesAndLen("../html/index.html")

				longline := strings.Repeat("a", 8192)
				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						longline,
					ExpectStatusCode: http.StatusBadRequest,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {contentLen},
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

				expectBody, contentLen := bytesAndLen("../html/index.html")

				longline := strings.Repeat("a", 1025)
				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:55000\r\n" +
						"Content-Length: 1025\r\n" +
						"\r\n" +
						longline,
					ExpectStatusCode: 413,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {contentLen},
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

				expectBody, contentLen := bytesAndLen("../html/index.html")

				longline := strings.Repeat("a", 8192)
				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:55000\r\n" +
						"Transfer-Encoding: chunked\r\n" +
						"\r\n" +
						longline,
					ExpectStatusCode: 400,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {contentLen},
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

				expectBody, contentLen := bytesAndLen("../html/index.html")

				longline := strings.Repeat("a", 1025)
				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:55000\r\n" +
						"Transfer-Encoding: chunked\r\n" +
						"\r\n" +
						"401\r\n" +
						longline +
						"\r\n",
					ExpectStatusCode: 413,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {contentLen},
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

				expectBody, contentLen := bytesAndLen("../html/index.html")

				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: localhost:55000\r\n" +
						"Transfer-Encoding: chunked\r\n" +
						"\r\n" +
						"4\r\n" +
						"Mozilla \r\n" +
						"\r\n",
					ExpectStatusCode: http.StatusBadRequest,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {contentLen},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
