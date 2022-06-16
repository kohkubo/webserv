package tests

import (
	"integration_test/httpresp"
	"integration_test/httptest"
	"net/http"
	"strconv"
)

var testLimitExpect = testCatergory{
	categoryName: "limit exept",
	config:       "integration_test/conf/test.conf",
	testCases: []testCase{
		{
			caseName: "limit_expect ok",
			test: func() bool {

				expectBody := fileToBytes("../html/index.html")
				contentLen := strconv.Itoa(len(expectBody))
				Port := "50003"
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
					ExpectBody: fileToBytes("../html/index.html"),
				})
				return clientA.DoAndCheck()
			},
		},
		{

			caseName: "limit_expect NG 405",
			test: func() bool {

				expectBody := fileToBytes("../html/index.html")
				contentLen := strconv.Itoa(len(expectBody))
				Port := "50003"
				Path := "/"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: Port,
					Request: `POST ` + Path + ` HTTP/1.1` + "\r\n" +
						`Host: localhost:` + Port + "\r\n" +
						`User-Agent: curl/7.79.1` + "\r\n" +
						`Accept: */*` + "\r\n" +
						`Content-Length: 9` + "\r\n" +
						`Content-Type: application/x-www-form-urlencoded` + "\r\n" +
						"\r\n" +
						"name=hoge",
					ExpectStatusCode: http.StatusMethodNotAllowed,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {contentLen},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: httpresp.ErrorBody(405),
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
