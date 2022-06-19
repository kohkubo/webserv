package tests

import (
	"integration_test/httpresp"
	"integration_test/httptest"
	"net/http"
)

var testIOMulti = testCatergory{
	categoryName: "IOMulti",
	config:       "integration_test/conf/iomulti.conf",
	testCases: []testCase{
		{
			caseName: "3client",
			test: func() bool {
				var clients []*httptest.Client
				port := "50000"
				expectStatusCode := 200
				expectBody := fileToBytes("html/index.html")
				// GET /
				clients = append(clients, httptest.NewClient(httptest.TestSource{
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
				}))

				port = "50000"
				expectStatusCode = 404
				expectBody = httpresp.ErrorBody(expectStatusCode)
				// GET /no_such_file_404
				clients = append(clients, httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /no_such_file_404 HTTP/1.1\r\n" +
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
				}))

				port = "50003"
				expectStatusCode = 405
				expectBody = httpresp.ErrorBody(expectStatusCode)
				// limit_expect NG 405
				clients = append(clients, httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "POST / HTTP/1.1" + "\r\n" +
						`Host: localhost:` + port + "\r\n" +
						"Connection: close\r\n" +
						`User-Agent: curl/7.79.1` + "\r\n" +
						`Accept: */*` + "\r\n" +
						`Content-Length: 9` + "\r\n" +
						`Content-Type: application/x-www-form-urlencoded` + "\r\n" +
						"\r\n" +
						"name=hoge",
					ExpectStatusCode: http.StatusMethodNotAllowed,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				}))

				for i, count := 0, 5; i < count; i++ {
					for _, c := range clients {
						switch i {
						case count - 1:
							c.SendRequestAll()
							c.RecvResponse()
							if ok := c.IsExpectedResponse(); !ok {
								return false
							}
						default:
							c.SendRequestRandom()
						}
					}
				}
				return true
			},
		},
	},
}
