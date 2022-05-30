package tests

import (
	"fmt"
	"integration_test/tester"
	"net/http"
)

// 複数クライアント(A, B, C)にコネクションと3分割したメッセージを用意して, ランダムに送信する
func TestIOMULT() {
	fmt.Println("IOMULT test")

	testHandler("3client", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5500",
			ReqPayload: []string{
				"GET /",
				" HTTP/1.1\r\nHost: localhost:5500\r\nUse",
				"r-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       HELLO_WORLD,
		})
		if err != nil {
			return false, err
		}
		clientB, err := tester.NewClient(&tester.Client{
			Port: "5001",
			ReqPayload: []string{
				"GET /nosuch HT",
				"TP/1.1\r\nHost: localhost:55",
				"00\r\nUser-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
			},
			ExpectStatusCode: http.StatusNotFound,
			ExpectHeader:     nil,
			ExpectBody:       content_404,
		})
		if err != nil {
			return false, err
		}
		clientC, err := tester.NewClient(&tester.Client{
			Port: "5001",
			ReqPayload: []string{
				"DELETE /nosuch HTTP/1.1\r",
				"\nHost: localhost:5500\r\nUser-Agent: Go-http-c",
				"lient/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
			},
			ExpectStatusCode: http.StatusNotFound,
			ExpectHeader:     nil,
			ExpectBody:       content_404,
		})
		if err != nil {
			return false, err
		}

		if err := clientA.SendPartialRequest(); err != nil {
			return false, err
		}
		if err := clientB.SendPartialRequest(); err != nil {
			return false, err
		}
		if err := clientC.SendPartialRequest(); err != nil {
			return false, err
		}
		if err := clientB.SendPartialRequest(); err != nil {
			return false, err
		}
		if err := clientA.SendPartialRequest(); err != nil {
			return false, err
		}
		if err := clientC.SendPartialRequest(); err != nil {
			return false, err
		}
		if err := clientB.SendPartialRequest(); err != nil {
			return false, err
		}

		clientB.RecvResponse()
		if !clientB.IsExpectedResponse() {
			return false, nil
		}

		if err := clientC.SendPartialRequest(); err != nil {
			return false, err
		}
		clientC.RecvResponse()
		if !clientC.IsExpectedResponse() {
			return false, nil
		}

		if err := clientA.SendPartialRequest(); err != nil {
			return false, err
		}
		clientA.RecvResponse()
		if !clientA.IsExpectedResponse() {
			return false, nil
		}
		return true, nil
	})

}
