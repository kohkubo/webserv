package tests

import (
	"fmt"
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
				expectStatusCode = 200
				expectBody = fileToBytes("html/index.html")
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
				expectStatusCode = 200
				expectBody = fileToBytes("html/index.html")
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

				// 5回ほどランダムにリクエスト送信
				for i, count := 0, 5; i < count; i++ {
					for _, c := range clients {
						var err error
						switch i {
						case count - 1:
							err = c.SendRequestAll()
						default:
							err = c.SendRequestRandom()
						}
						if err != nil {
							fmt.Println(err)
							return false
						}
					}
				}
				// レスポンス確認
				for _, c := range clients {
					if err := c.RecvResponse(); err != nil {
						fmt.Println(err)
						return false
					}
					ok, err := c.IsExpectedResponse()
					if err != nil || !ok {
						fmt.Println(err)
						return false
					}
				}
				return true
			},
		},
		//	{
		//		caseName: "multiclient",
		//		test: func() bool {

		//expectBody := fileToBytes("html/index.html")
		//
		//			baseClient := httptest.TestSource{
		//				port := "55000"
		//Port: port,
		//				Request:
		//					"GET /",
		//					" HTTP/1.1\r\nHost: localhost:55000\r\nUse",
		//					"r-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n"+
		//				},
		//				ExpectStatusCode: expectStatusCode,
		//									ExpectHeader: http.Header{
		//	"Connection":     {"close"},
		//	"Content-Length": {lenStr(expectBody)},
		//	"Content-Type":   {"text/html"},
		//},
		//				ExpectBody:       fileToBytes("html/index.html"),
		//			}
		//			numOfClient := 10
		//			// 10247で落ちた テストケース追加 goroutine 長いやつ用にフラグ使い分け
		//			// 現状だとエラーが返ってくるのでテストとして修正すべき
		//			var clients []*httptest.TestSource
		//			for i := 0; i < numOfClient; i++ {
		//				clients = append(clients, httptest.NewClient(baseClient))
		//			}
		//			for cnt := 0; cnt < 3; cnt++ {
		//				for i := 0; i < numOfClient; i++ {
		//if err := 					clients[i].SendRequestRandom(); err != nil
		//				}
		//			}
		//			for i := 0; i < numOfClient; i++ {
		//				clients[i].RecvResponse()
		//				if ok := clients[i].IsExpectedResponse(); !ok {
		//					return false
		//				}
		//			}
		//			return true
		//		},
		//	},
	},
}
