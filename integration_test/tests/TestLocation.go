package tests

import (
	"integration_test/httptest"
	"net/http"
	"strconv"
)

var testLocation = testCatergory{
	categoryName: "location",
	config:       "integration_test/conf/test.conf",
	testCases: []testCase{
		{
			caseName: "locationでdir1を指定できているか",
			test: func() bool {

				expectBody := fileToBytes("../html/index.html")
				contentLen := strconv.Itoa(len(expectBody))
				clientA := httptest.NewClient(httptest.TestSource{
					Port: "50000",
					Request: "GET /dir1/ HTTP/1.1\r\n" +
						"Host: localhost:50000\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: 200,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {contentLen},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: fileToBytes("../html/dir1/index.html"),
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "rootディレクティブが反映されるか",
			test: func() bool {

				expectBody := fileToBytes("../html/index.html")
				contentLen := strconv.Itoa(len(expectBody))
				Port := "50001"
				Path := "/"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: Port,
					Request: "GET " + Path + " HTTP/1.1\r\n" +
						"Host: localhost:" + Port + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: 200,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {contentLen},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: fileToBytes("../html/dir1/index.html"),
				})
				return clientA.DoAndCheck()
			},
		},
		{

			caseName: "index指定ができているか",
			test: func() bool {

				expectBody := fileToBytes("../html/index.html")
				contentLen := strconv.Itoa(len(expectBody))
				Port := "50002"
				Path := "/"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: Port,
					Request: "GET " + Path + " HTTP/1.1\r\n" +
						"Host: localhost:" + Port + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: 200,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {contentLen},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: fileToBytes("../html/dir1/index2.html"),
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "index指定ができているか",
			test: func() bool {

				expectBody := fileToBytes("../html/index.html")
				contentLen := strconv.Itoa(len(expectBody))
				Port := "50002"
				Path := "/"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: Port,
					Request: "GET " + Path + " HTTP/1.1\r\n" +
						"Host: localhost:" + Port + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: 200,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {contentLen},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: fileToBytes("../html/dir1/index2.html"),
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
