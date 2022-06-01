package tests

import (
	"integration_test/tester"
	"net/http"
)

func TestGET() {
	testHandler("simple_5500", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5500",
			ReqPayload: []string{
				"GET / HTTP/1.1\r\n",
				"Host: localhost:5500\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       HELLO_WORLD,
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})

	testHandler("simple_5001", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5001",
			ReqPayload: []string{
				"GET / HTTP/1.1\r\n",
				"Host: localhost:5001\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       HELLO_WORLD,
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})

	testHandler("no_such_file", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5001",
			ReqPayload: []string{
				"GET /no_such_file HTTP/1.1\r\n",
				"Host: localhost:5001\r\n",
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

}
