package tests

import (
	"context"
	"fmt"
	"integration_test/httptest"
	"net/http"
	"os"

	"golang.org/x/sync/errgroup"
)

var testMultiConnection = testCatergory{
	categoryName: "MultiConnection",
	config:       "integration_test/conf/iomulti.conf",
	testCases: []testCase{
		{
			caseName: "parallel connection",
			test: func() bool {
				port := "50000"
				expectStatusCode := 200
				expectBody := fileToBytes("html/index.html")
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
				eg, ctx := errgroup.WithContext(context.Background())
				count := 100 //150くらいで落ちる, 理由はSOMAXCONNを超えて接続拒否されたことによるものと認識
				for i := 0; i < count; i++ {
					i := i
					eg.Go(func() error {
						select {
						case <-ctx.Done():
						default:
							c := httptest.NewClient(baseSource)
							if !c.DoAndCheck() {
								return fmt.Errorf("goroutine number %v: unexpected response", i)
							}
						}
						return nil
					})
				}
				if err := eg.Wait(); err != nil {
					fmt.Fprintln(os.Stderr, err)
					return false
				}
				return true
			},
		},
		//{
		//	caseName: "parallel connection",
		//	test: func() bool {
		//		port := "50000"
		//		expectStatusCode := 200
		//		expectBody := fileToBytes("html/index.html")
		//		// GET /
		//		baseSource := httptest.TestSource{
		//			Port: port,
		//			Request: "GET / HTTP/1.1\r\n" +
		//				"Host: localhost:" + port + "\r\n" +
		//				"Connection: close\r\n" +
		//				"User-Agent: curl/7.79.1\r\n" +
		//				`Accept: */*` + "\r\n" +
		//				"\r\n",
		//			ExpectStatusCode: expectStatusCode,
		//			ExpectHeader: http.Header{
		//				"Connection":     {"close"},
		//				"Content-Length": {lenStr(expectBody)},
		//				"Content-Type":   {"text/html"},
		//			},
		//			ExpectBody: expectBody,
		//		}
		//		// TODO: loopでconnectしていたときはcount=10247で落ちた
		//		// TODO: goroutineで並行connectするとcount=150くらいで落ちる
		//		// TODO: connection確立時のエラーを拾うなら直でconnection関数使う
		//		eg, ctx := errgroup.WithContext(context.Background())
		//		for i, count := 0, 100; i < count; i++ {
		//			i := i
		//			eg.Go(func() error {
		//				select {
		//				case <-ctx.Done():
		//				default:
		//					c := httptest.NewClient(baseSource)
		//					c.SendRequestAll()
		//					c.RecvResponse()
		//					if ok := c.IsExpectedResponse(); !ok {
		//						return fmt.Errorf("goroutine number %v: unexpected response", i)
		//					}
		//				}
		//				return nil
		//			})
		//		}
		//		if err := eg.Wait(); err != nil {
		//			fmt.Fprintln(os.Stderr, err)
		//			return false
		//		}
		//		return true
		//	},
		//},
	},
}
