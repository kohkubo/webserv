package tests

import (
	"integration_test/httptest"
	"net/http"
	"strconv"
)

var testServerName = testCatergory{
	categoryName: "servername",
	config:       "integration_test/conf/server_name.conf",
	testCases: []testCase{
		{
			caseName: "match_hoge",
			test: func() bool {
				port := "50001"
				expectBody := []byte("index in dir1")
				contentLen := strconv.Itoa(len(expectBody))
				expectStatusCode := 200
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
						"Content-Length": {contentLen},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "match_fuga",
			test: func() bool {
				port := "50001"
				expectBody := []byte("index in dir2")
				contentLen := strconv.Itoa(len(expectBody))
				expectStatusCode := 200
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
						"Content-Length": {contentLen},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "no_match",
			test: func() bool {
				port := "50001"
				expectBody := []byte("index in server_name")
				contentLen := strconv.Itoa(len(expectBody))
				expectStatusCode := 200
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
						"Host: no_such_host.com:" + port + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
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
