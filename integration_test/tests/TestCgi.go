package tests

import (
	"integration_test/httptest"
	"net/http"
	"strconv"
)

var testCgi = testCatergory{
	categoryName: "cgi",
	config:       "integration_test/conf/test.conf",
	testCases: []testCase{
		{
			caseName: "5000_cgi_get_normal",
			test: func() bool {

				expectBody := fileToBytes("../html/index.html")
				contentLen := strconv.Itoa(len(expectBody))
				Port := "50000"
				Path := "/cgi.sh"
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
					ExpectBody: []byte(
						`###########################
name=
blood=
text=
example=
###########################
`),
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
