package tests

import (
	"fmt"
	"integration_test/exe"
	"integration_test/tester"
	"net/http"
)

var testLocation = TestCatergory{
	Name:   "location",
	Config: "integration_test/conf/test.conf",
	TestCases: []TestCase{
		{
			Name: "locationでdir1を指定できているか",
			Test: func() (bool, error) {
				ExpectBody, err := exe.FileToBytes("../html/dir1/index.html")
				if err != nil {
					return false, fmt.Errorf("failt to get bytes from file")
				}
				clientA, err := tester.NewClient(&tester.Client{
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
					ExpectBody:       ExpectBody,
				})
				if err != nil {
					return false, err
				}
				return clientA.DoAndCheck()
			},
		},
		{

			Name: "rootディレクティブが反映されるか",
			Test: func() (bool, error) {
				ExpectBody, err := exe.FileToBytes("../html/dir1/index.html")
				if err != nil {
					return false, fmt.Errorf("failt to get bytes from file")
				}
				Port := "5001"
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
					ExpectBody:       ExpectBody,
				})
				if err != nil {
					return false, err
				}
				return clientA.DoAndCheck()
			},
		},
		{

			Name: "index指定ができているか",
			Test: func() (bool, error) {
				ExpectBody, err := exe.FileToBytes("../html/dir1/index2.html")
				if err != nil {
					return false, fmt.Errorf("failt to get bytes from file")
				}
				Port := "5002"
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
					ExpectBody:       ExpectBody,
				})
				if err != nil {
					return false, err
				}
				return clientA.DoAndCheck()
			},
		},
		{
			Name: "index指定ができているか",
			Test: func() (bool, error) {
				ExpectBody, err := exe.FileToBytes("../html/dir1/index2.html")
				if err != nil {
					return false, fmt.Errorf("failt to get bytes from file")
				}
				Port := "5002"
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
					ExpectBody:       ExpectBody,
				})
				if err != nil {
					return false, err
				}
				return clientA.DoAndCheck()
			},
		},
	},
}
