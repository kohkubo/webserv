package tests

import (
	"integration_test/response"
	"integration_test/tester"
	"integration_test/tests/utils"
	"net/http"
)

var testLimitExpect = testCatergory{
	name:   "limit exept",
	config: "integration_test/conf/test.conf",
	testCases: []testCase{
		{
			name: "limit_expect ok",
			test: func() (bool, error) {
				Port := "5003"
				Path := "/"
				clientA, err := tester.NewClient(&tester.Client{
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
					ExpectBody:       utils.FileToBytes("../html/index.html"),
				})
				if err != nil {
					return false, err
				}
				return clientA.DoAndCheck()
			},
		},
		{

			name: "limit_expect NG 405",
			test: func() (bool, error) {
				Port := "5003"
				Path := "/"
				clientA, err := tester.NewClient(&tester.Client{
					Port: Port,
					ReqPayload: []string{
						`POST ` + Path + ` HTTP/1.1` + "\r\n",
						`Host: localhost:` + Port + "\r\n",
						`User-Agent: curl/7.79.1` + "\r\n",
						`Accept: */*` + "\r\n",
						`Content-Length: 9` + "\r\n",
						`Content-Type: application/x-www-form-urlencoded` + "\r\n",
						"\r\n",
						"name=hoge",
					},
					ExpectStatusCode: 405,
					ExpectHeader:     nil,
					ExpectBody:       response.Content_405,
				})
				if err != nil {
					return false, err
				}
				return clientA.DoAndCheck()
			},
		},
	},
}
