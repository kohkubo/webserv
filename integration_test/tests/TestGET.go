package tests

import (
	"integration_test/tester"
	"net/http"
	"os"
)

func TestGET() {
	testHandler("GET / ", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5000",
			ReqPayload: []string{
				"GET / HTTP/1.1\r\n",
				"Host: localhost:5000\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       fileToBytes("../html/index.html"),
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})
	testHandler("GET /dir1/index2.html ", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5000",
			ReqPayload: []string{
				"GET /dir1/index2.html HTTP/1.1\r\n",
				"Host: localhost:5000\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       fileToBytes("../html/dir1/index2.html"),
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})
	testHandler("GET /no_such_file_404", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5000",
			ReqPayload: []string{
				"GET /no_such_file_404 HTTP/1.1\r\n",
				"Host: localhost:5000\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusNotFound,
			ExpectHeader:     nil,
			ExpectBody:       content_404,
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})


	// TODO: ファイル直接指定の場合のアクセス権限test

	testHandler("index解決後のアクセス権限確認test", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5000",
			ReqPayload: []string{
				"GET / HTTP/1.1\r\n",
				"Host: localhost:5000\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: 403,
			ExpectHeader:     nil,
			ExpectBody:       content_403,
		})
		if err != nil {
			return false, err
		}
		os.Chmod("../html/index.html", 000)
		ok, err := clientA.Test()
		os.Chmod("../html/index.html", 0755)
		return ok, err
	})
}
