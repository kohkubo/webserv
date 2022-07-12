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
				count := 150 //150くらいでエラー => RecvResponse: readParseResponse: read tcp 127.0.0.1:58012->127.0.0.1:50000: read: connection reset by peer
				// => 何かしらのエラーでサーバから接続をリセットされたときに出るエラー
				// SOMAXCONNを超えて接続拒否されたことによるものと認識していたが, Responseをreadする時点で起きている
				// webserbが用意していないエラー => もっと下のレイヤーで起きたもの
				// TODO: 上記のエラーの種類がわからない(errors.Is(syscall.ECONNRESET, err)ではなかった)のと
				//       それが確かに起きるはずのエラーなのかわかっていないので, エラーを拾っての処理は保留
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
		{
			caseName: "multi connection",
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
				count := 1000 // 10240あたりでエラー => RecvResponse: readParseResponse: unexpected EOF
				// TODO: errors.Is(io.ErrUnexpectedEOF, err)で判定できるが, 上のテストと同じくエラー拾っての処理は保留
				var clients []*httptest.Client
				for i := 0; i < count; i++ {
					c := httptest.NewClient(baseSource)
					clients = append(clients, c)
				}
				for i := 0; i < count; i++ {
					if !clients[i].DoAndCheck() {
						return false
					}
				}
				return true
			},
		},
	},
}
