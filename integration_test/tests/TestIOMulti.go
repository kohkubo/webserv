package tests

import (
	"integration_test/response"
	"integration_test/tester"
	"net/http"
)

// 複数クライアント(A, B, C)にコネクションと3分割したメッセージを用意して, ランダムに送信する
var testIOMulti = testCatergory{
	categoryName: "IOmulti",
	config:       "integration_test/conf/webserv.conf",
	testCases: []testCase{
		{
			caseName: "3client",
			test: func() bool {
				clientA := tester.NewClient(tester.Client{
					Port: "5500",
					ReqPayload: []string{
						"GET /",
						" HTTP/1.1\r\nHost: localhost:5500\r\nUse",
						"r-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       fileToBytes("../html/index.html"),
				})

				clientB := tester.NewClient(tester.Client{
					Port: "5001",
					ReqPayload: []string{
						"GET /nosuch HT",
						"TP/1.1\r\nHost: localhost:55",
						"00\r\nUser-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
					},
					ExpectStatusCode: http.StatusNotFound,
					ExpectHeader:     nil,
					ExpectBody:       response.Content_404,
				})

				clientC := tester.NewClient(tester.Client{
					Port: "5001",
					ReqPayload: []string{
						"DELETE /nosuch HTTP/1.1\r",
						"\nHost: localhost:5500\r\nUser-Agent: Go-http-c",
						"lient/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
					},
					ExpectStatusCode: http.StatusNotFound,
					ExpectHeader:     nil,
					ExpectBody:       response.Content_404,
				})

				clientA.SendPartialRequest()
				clientB.SendPartialRequest()
				clientC.SendPartialRequest()
				clientB.SendPartialRequest()
				clientA.SendPartialRequest()
				clientC.SendPartialRequest()
				clientB.SendPartialRequest()

				clientB.RecvResponse()
				if ok := clientB.IsExpectedResponse(); !ok {
					return false
				}

				clientC.SendPartialRequest()
				clientC.RecvResponse()
				if ok := clientC.IsExpectedResponse(); !ok {
					return false
				}

				clientA.SendPartialRequest()
				clientA.RecvResponse()
				if ok := clientA.IsExpectedResponse(); !ok {
					return false
				}
				return true
			},
		},
		{
			caseName: "multiclient",
			test: func() bool {
				baseClient := tester.Client{
					Port: "5500",
					ReqPayload: []string{
						"GET /",
						" HTTP/1.1\r\nHost: localhost:5500\r\nUse",
						"r-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       fileToBytes("../html/index.html"),
				}
				numOfClient := 5000 // 時間がかかるので一旦10, 5000とかでもいけたけど。。。いけて良いのか?
				var clients []*tester.Client
				for i := 0; i < numOfClient; i++ {
					clients = append(clients, tester.NewClient(baseClient))
				}
				for cnt := 0; cnt < 3; cnt++ {
					for i := 0; i < numOfClient; i++ {
						clients[i].SendPartialRequest()
					}
				}
				for i := 0; i < numOfClient; i++ {
					clients[i].RecvResponse()
					if ok := clients[i].IsExpectedResponse(); !ok {
						return false
					}
				}
				return true
			},
		},
	},
}
