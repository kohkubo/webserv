package main

import (
	"fmt"
)

func testGET() {
	fmt.Println("GET test")

	testHandler("simple_5500", func() (bool, error) {
		clientA := NewClient(&Client{
			Port: "5500",
			ReqPayload: []string{
				"GET / HTTP/1.1\r\n",
				"Host: localhost:5500\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectHeader: nil,
			ExpectBody:   FileToBytes(HELLO_WORLD_PAGE),
		})
		return clientA.isTestOK(), nil
	})

	testHandler("simple_5001", func() (bool, error) {
		clientA := NewClient(&Client{
			Port: "5001",
			ReqPayload: []string{
				"GET / HTTP/1.1\r\n",
				"Host: localhost:5001\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectHeader: nil,
			ExpectBody:   FileToBytes(HELLO_WORLD_PAGE),
		})
		return clientA.isTestOK(), nil
	})

	testHandler("no_such_file", func() (bool, error) {
		clientA := NewClient(&Client{
			Port: "5001",
			ReqPayload: []string{
				"GET /no_such_file HTTP/1.1\r\n",
				"Host: localhost:5001\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectHeader: nil,
			ExpectBody:   FileToBytes(NOT_FOUND_PAGE),
		})
		return clientA.isTestOK(), nil
	})
}
