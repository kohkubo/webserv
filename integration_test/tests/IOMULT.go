package tests

import (
	"fmt"
	"integration_test/tester"
)

// 複数クライアント(A, B, C)にコネクションと3分割したメッセージを用意して, ランダムに送信する
func TestIOMULT() {
	fmt.Println("IOMULT test")

	testHandler("3client", func() (bool, error) {
		clientA := tester.NewClient(&tester.Client{
			Port: "5500",
			ReqPayload: []string{
				"GET /",
				" HTTP/1.1\r\nHost: localhost:5500\r\nUse",
				"r-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
			},
			ExpectHeader: nil,
			ExpectBody:   HELLO_WORLD,
		})
		clientB := tester.NewClient(&tester.Client{
			Port: "5001",
			ReqPayload: []string{
				"GET /nosuch HT",
				"TP/1.1\r\nHost: localhost:55",
				"00\r\nUser-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
			},
			ExpectHeader: nil,
			ExpectBody:   NOT_FOUND,
		})
		clientC := tester.NewClient(&tester.Client{
			Port: "5001",
			ReqPayload: []string{
				"DELETE /nosuch HTTP/1.1\r",
				"\nHost: localhost:5500\r\nUser-Agent: Go-http-c",
				"lient/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
			},
			ExpectHeader: nil,
			ExpectBody:   NOT_FOUND,
		})

		clientA.SendPartialRequest()
		clientB.SendPartialRequest()
		clientC.SendPartialRequest()
		clientB.SendPartialRequest()
		clientA.SendPartialRequest()
		clientC.SendPartialRequest()
		clientB.SendPartialRequest()

		clientB.RecvResponse()
		if !clientB.IsExpectedResponse() {
			return false, nil
		}

		clientC.SendPartialRequest()
		clientC.RecvResponse()
		if !clientC.IsExpectedResponse() {
			return false, nil
		}

		clientA.SendPartialRequest()
		clientA.RecvResponse()
		if !clientA.IsExpectedResponse() {
			return false, nil
		}
		return true, nil
	})

}
