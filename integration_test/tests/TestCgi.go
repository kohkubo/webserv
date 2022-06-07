package tests

import (
	"integration_test/tester"
	"net/http"
)

var testCgi = testCatergory{
	name:   "cgi",
	config: "integration_test/conf/test.conf",
	testCases: []testCase{
		{
			name: "5000_cgi_get_normal",
			test: func() bool {
				Port := "5000"
				Path := "/cgi.sh"
				clientA := tester.NewClient(&tester.Client{
					Port: Port,
					ReqPayload: []string{
						"GET " + Path + " HTTP/1.1\r\n",
						"Host: localhost:" + Port + "\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
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
