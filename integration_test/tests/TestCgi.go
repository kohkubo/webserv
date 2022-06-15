package tests

import (
	"integration_test/tester"
	"net/http"
)

var testCgi = testCatergory{
	categoryName: "cgi",
	config:       "integration_test/conf/test.conf",
	testCases: []testCase{
		{
			caseName: "5000_cgi_get_normal",
			test: func() bool {
				Port := "50000"
				clientA := tester.NewClient(tester.Client{
					Port: Port,
					ReqPayload: []string{
						"GET /cgi_test.py?name=taro&blood=A&text=string HTTP/1.1\r\n",
						"Host: localhost:" + Port + "\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody: []byte(
						`name= taro
blood= A
text= string
`,
					),
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
