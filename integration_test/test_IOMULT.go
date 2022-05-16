package main

import (
	"fmt"
	"log"
	"os"
)

// 複数クライアント(A, B, C)にコネクションと3分割したメッセージを用意して, ランダムに送信する
func testIOMULT() {
	fmt.Println("IOMULT test")
	testHandler("3client", func() (bool, error) {
		clientA := &Client{
			Port: "5500",
			ReqPayload: []string{
				"GET /",
				" HTTP/1.1\r\nHost: localhost:5500\r\nUse",
				"r-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
			},
			Method:       "GET",
			ExpectHeader: nil,
			ExpectBody:   FileToBytes(HELLO_WORLD_PAGE),
		}
		clientB := &Client{
			Port: "5001",
			ReqPayload: []string{
				"GET /nosuch HT",
				"TP/1.1\r\nHost: localhost:55",
				"00\r\nUser-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
			},
			Method:       "GET",
			ExpectHeader: nil,
			ExpectBody:   FileToBytes(NOT_FOUND_PAGE),
		}
		clientC := &Client{
			Port: "5001",
			ReqPayload: []string{
				"DELETE /nosuch HTTP/1.1\r",
				"\nHost: localhost:5500\r\nUser-Agent: Go-http-c",
				"lient/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
			},
			Method:       "DELETE",
			ExpectHeader: nil,
			ExpectBody:   FileToBytes(NOT_FOUND_PAGE),
		}
		connectAll(clientA, clientB, clientC)

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

// for color print
const (
	red   = "\033[31m"
	green = "\033[32m"
	reset = "\033[0m"
)

// テスト関数を渡してその結果に合わせたメッセージを出力する関数です
func testHandler(name string, test func() (bool, error)) {
	fmt.Print("[ " + name + " ] ")
	ok, err := test()
	if err != nil {
		log.Fatalf("erro occured!: %v", err)
	}
	if ok {
		fmt.Println(green, "ok", reset)
	} else {
		fmt.Println(red, "error", reset)
		os.Exit(1) // TODO: テスト全体でエラーがあれば最後にexit(1)する
	}
}
