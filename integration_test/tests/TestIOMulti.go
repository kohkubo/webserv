package tests

// 複数クライアント(A, B, C)にコネクションと3分割したメッセージを用意して, ランダムに送信する
//var testIOMulti = testCatergory{
//categoryName: "IOmulti",
//config:       "integration_test/conf/webserv.conf",
//testCases: []testCase{
//	{
//		caseName: "3client",
//		test: func() bool {

//expectBody, contentLen := bytesAndLen("html/index.html")
//
//			clientA := httptest.NewClient(httptest.TestSource{
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
//	"Content-Length": {contentLen},
//	"Content-Type":   {"text/html"},
//},
//				ExpectBody:       bytesAndLen("html/index.html"),
//			})

//			clientB := httptest.NewClient(httptest.TestSource{
//				Port: "50001",
//				Request:
//					"GET /nosuch HT",
//					"TP/1.1\r\nHost: localhost:55",
//					"00\r\nUser-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n"+
//				},
//				ExpectStatusCode: 404,
//									ExpectHeader: http.Header{
//	"Connection":     {"close"},
//	"Content-Length": {contentLen},
//	"Content-Type":   {"text/html"},
//},
//				ExpectBody:       errBytesAndLen(404),
//			})

//			clientC := httptest.NewClient(httptest.TestSource{
//				Port: "50001",
//				Request:
//					"DELETE /nosuch HTTP/1.1\r",
//					"\nHost: localhost:55000\r\nUser-Agent: Go-http-c",
//					"lient/1.1\r\nAccept-Encoding: gzip\r\n\r\n"+
//				},
//				ExpectStatusCode: 404,
//									ExpectHeader: http.Header{
//	"Connection":     {"close"},
//	"Content-Length": {contentLen},
//	"Content-Type":   {"text/html"},
//},
//				ExpectBody:       errBytesAndLen(404),
//			})

//			clientA.SendPartialRequest()
//			clientB.SendPartialRequest()
//			clientC.SendPartialRequest()
//			clientB.SendPartialRequest()
//			clientA.SendPartialRequest()
//			clientC.SendPartialRequest()
//			clientB.SendPartialRequest()

//			clientB.RecvResponse()
//			if ok := clientB.IsExpectedResponse(); !ok {
//				return false
//			}

//			clientC.SendPartialRequest()
//			clientC.RecvResponse()
//			if ok := clientC.IsExpectedResponse(); !ok {
//				return false
//			}

//			clientA.SendPartialRequest()
//			clientA.RecvResponse()
//			if ok := clientA.IsExpectedResponse(); !ok {
//				return false
//			}
//			return true
//		},
//	},
//	{
//		caseName: "multiclient",
//		test: func() bool {

//expectBody, contentLen := bytesAndLen("html/index.html")
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
//	"Content-Length": {contentLen},
//	"Content-Type":   {"text/html"},
//},
//				ExpectBody:       bytesAndLen("html/index.html"),
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
//					clients[i].SendPartialRequest()
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
//},
//}
