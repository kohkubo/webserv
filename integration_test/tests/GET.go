package tests

import (
	"fmt"
	"integration_test/tester"
	"net/http"
)

func TestGET() {
	fmt.Println("GET test")

	testHandler("simple_5500", func() (bool, error) {
		clientA := tester.NewClient(&tester.Client{
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
		return clientA.Test(), nil
	})

	testHandler("simple_5001", func() (bool, error) {
		clientA := tester.NewClient(&tester.Client{
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
		return clientA.Test(), nil
	})

	testHandler("no_such_file", func() (bool, error) {
		clientA := tester.NewClient(&tester.Client{
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
			ExpectBody:       NOT_FOUND,
		})
		return clientA.Test(), nil
	})

}
