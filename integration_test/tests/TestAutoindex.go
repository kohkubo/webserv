package tests

import (
	"integration_test/exe"
	"integration_test/tester"
	"net/http"
)

func TestAutoindex() {

	// 環境によってdirectoryのlistされる順番が違うみたいなのでレスポンスボディ自体を確認するのは保留
	exe.SmallHandler("simple", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5001",
			ReqPayload: []string{
				"GET /autoindex/ HTTP/1.1\r\n",
				"Host: localhost:5001\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       nil,
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})

	exe.SmallHandler("forbidden", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5001",
			ReqPayload: []string{
				"GET /autoindex/dir2/ HTTP/1.1\r\n",
				"Host: localhost:5001\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusForbidden,
			ExpectHeader:     nil,
			ExpectBody:       nil,
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})

	exe.SmallHandler("index_priority", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5001",
			ReqPayload: []string{
				"GET /autoindex/dir1/ HTTP/1.1\r\n",
				"Host: localhost:5001\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       []byte("in test_autoindex/dir1"),
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})
}