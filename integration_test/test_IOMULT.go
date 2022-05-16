package main

import (
	"fmt"
)

// 複数クライアント(A, B, C)にコネクションと3分割したメッセージを用意して, ランダムに送信する
func testIOMULT() {
	fmt.Println("IOMULT test")
	testHandler("3client", func() (bool, error) {
		clientA := NewClient(&Client{
			Port: "5500",
			ReqPayload: []string{
				"GET /",
				" HTTP/1.1\r\nHost: localhost:5500\r\nUse",
				"r-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
			},
			Method:       "GET",
			ExpectHeader: nil,
			ExpectBody:   FileToBytes(HELLO_WORLD_PAGE),
		})
		clientB := NewClient(&Client{
			Port: "5001",
			ReqPayload: []string{
				"GET /nosuch HT",
				"TP/1.1\r\nHost: localhost:55",
				"00\r\nUser-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
			},
			Method:       "GET",
			ExpectHeader: nil,
			ExpectBody:   FileToBytes(NOT_FOUND_PAGE),
		})
		clientC := NewClient(&Client{
			Port: "5001",
			ReqPayload: []string{
				"DELETE /nosuch HTTP/1.1\r",
				"\nHost: localhost:5500\r\nUser-Agent: Go-http-c",
				"lient/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
			},
			Method:       "DELETE",
			ExpectHeader: nil,
			ExpectBody:   FileToBytes(NOT_FOUND_PAGE),
		})

		clientA.sendPartialRequest()
		clientB.sendPartialRequest()
		clientC.sendPartialRequest()
		clientB.sendPartialRequest()
		clientA.sendPartialRequest()
		clientC.sendPartialRequest()
		clientB.sendPartialRequest()
		if !clientB.isExpectedResult() {
			return false, nil
		}
		clientC.sendPartialRequest()
		if !clientC.isExpectedResult() {
			return false, nil
		}
		clientA.sendPartialRequest()
		if !clientA.isExpectedResult() {
			return false, nil
		}
		return true, nil
	})
}
