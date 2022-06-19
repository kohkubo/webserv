package tests

import (
	"integration_test/httptest"
	"net/http"
)

// connection確立時にエラーを拾うなら直でconnection関数使う
var testMultiConnection = testCatergory{
	categoryName: "MultiConnection",
	config:       "integration_test/conf/iomulti.conf",
	testCases: []testCase{
		{
			caseName: "multiclient",
			test: func() bool {
				port := "50000"
				expectStatusCode := 200
				expectBody := fileToBytes("html/index.html")
				// GET /
				baseSource := httptest.TestSource{
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
				}
				numOfClient := 10
				// 10247で落ちた テストケース追加 goroutine 長いやつ用にフラグ使い分け
				// 現状だとエラーが返ってくるのでテストとして修正すべき
				var clients []*httptest.Client
				for i := 0; i < numOfClient; i++ {
					clients = append(clients, httptest.NewClient(baseSource))
				}
				for _, c := range clients {
					c.SendRequestAll()
					c.RecvResponse()
					if ok := c.IsExpectedResponse(); !ok {
						return false
					}
				}
				return true
			},
		},
	},
}
