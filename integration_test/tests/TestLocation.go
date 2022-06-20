package tests

import (
	"integration_test/httptest"
	"net/http"
)

var testLocation = testCatergory{
	categoryName: "location",
	config:       "integration_test/conf/location.conf",
	testCases: []testCase{
		{
			caseName: "最長マッチチェック 0",
			test: func() bool {
				port := "50000"
				expectStatusCode := 200
				expectBody := fileToBytes("html/test_location/dummy_root/index.html")
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET / HTTP/1.1\r\n" +
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
		{
			caseName: "最長マッチチェック 1",
			test: func() bool {
				port := "50000"
				expectStatusCode := 200
				expectBody := fileToBytes("html/test_location/location/location1/index.html")
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /location1/ HTTP/1.1\r\n" +
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
		{
			caseName: "最長マッチチェック 2",
			test: func() bool {
				port := "50000"
				expectStatusCode := 200
				expectBody := fileToBytes("html/test_location/location/location2/index.html")
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /location/location2/ HTTP/1.1\r\n" +
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
		{
			caseName: "最長マッチチェック 3",
			test: func() bool {
				port := "50000"
				expectStatusCode := 200
				expectBody := fileToBytes("html/test_location/another_location/location1/dir1/hoge.html")
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /location1/dir1/ HTTP/1.1\r\n" +
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
