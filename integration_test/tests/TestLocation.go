package tests

import (
	"integration_test/tester"
	"net/http"
)

var testLocation = testCatergory{
	categoryName: "location",
	config:       "integration_test/conf/test.conf",
	testCases: []testCase{
		{
			caseName: "locationでdir1を指定できているか",
			test: func() bool {
				clientA := tester.InitClient(&tester.Client{
					Port: "5000",
					ReqPayload: []string{
						"GET /dir1/ HTTP/1.1\r\n",
						"Host: localhost:5000\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       fileToBytes("../html/dir1/index.html"),
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "rootディレクティブが反映されるか",
			test: func() bool {
				Port := "5001"
				Path := "/"
				clientA := tester.InitClient(&tester.Client{
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
					ExpectBody:       fileToBytes("../html/dir1/index.html"),
				})
				return clientA.DoAndCheck()
			},
		},
		{

			caseName: "index指定ができているか",
			test: func() bool {
				Port := "5002"
				Path := "/"
				clientA := tester.InitClient(&tester.Client{
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
					ExpectBody:       fileToBytes("../html/dir1/index2.html"),
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "index指定ができているか",
			test: func() bool {
				Port := "5002"
				Path := "/"
				clientA := tester.InitClient(&tester.Client{
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
					ExpectBody:       fileToBytes("../html/dir1/index2.html"),
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
