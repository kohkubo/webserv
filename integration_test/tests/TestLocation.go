package tests

import (
	"integration_test/httptest"
	"net/http"
)

var testLocation = testCatergory{
	categoryName: "location",
	config:       "integration_test/conf/test.conf",
	testCases: []testCase{
		{
			caseName: "locationでdir1を指定できているか",
			test: func() bool {
				port := "50000"
				expectStatusCode := 200
				expectBody, contentLen := bytesAndLen("html/dir1/index.html")
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /dir1/ HTTP/1.1\r\n" +
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
			caseName: "rootディレクティブが反映されるか",
			test: func() bool {
				expectStatusCode := 200
				expectBody, contentLen := bytesAndLen("html/dir1/index.html")
				Port := "50001"
				Path := "/"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: Port,
					Request: "GET " + Path + " HTTP/1.1\r\n" +
						"Host: localhost:" + Port + "\r\n" +
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

			caseName: "index指定ができているか",
			test: func() bool {

				expectStatusCode := 200
				expectBody, contentLen := bytesAndLen("html/dir1/index2.html")
				Port := "50002"
				Path := "/"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: Port,
					Request: "GET " + Path + " HTTP/1.1\r\n" +
						"Host: localhost:" + Port + "\r\n" +
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
			caseName: "index指定ができているか",
			test: func() bool {

				expectStatusCode := 200
				expectBody, contentLen := bytesAndLen("html/dir1/index2.html")
				Port := "50002"
				Path := "/"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: Port,
					Request: "GET " + Path + " HTTP/1.1\r\n" +
						"Host: localhost:" + Port + "\r\n" +
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
