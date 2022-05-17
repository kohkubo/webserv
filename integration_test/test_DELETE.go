package main

import (
	"fmt"
)

func testDELETE() {
	fmt.Println("DELETE test")

	testHandler("no_such_file", func() (bool, error) {
		clientA := NewClient(&Client{
			Port: "5500",
			ReqPayload: []string{
				"DELETE /no_such_file HTTP/1.1\r\n",
				"Host: localhost:5500\r\n",
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
