package tests

import (
	"integration_test/tester"
	"net/http"
)

func TestServer_name() {
	testHandler("match_hoge", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5001",
			ReqPayload: []string{
				"GET / HTTP/1.1\r\n",
				"Host: hoge.com:5001\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       []byte("index in dir1"),
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})

	testHandler("match_fuga", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5001",
			ReqPayload: []string{
				"GET / HTTP/1.1\r\n",
				"Host: fuga.com:5001\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       []byte("index in dir2"),
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})

	testHandler("no_match", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5001",
			ReqPayload: []string{
				"GET / HTTP/1.1\r\n",
				"Host: no_such_host.com:5001\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       []byte("index in server_name"),
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})
}