package tests

import (
	"fmt"
	"integration_test/response"
	"integration_test/tester"
	"net/http"
)

// 複数クライアント(A, B, C)にコネクションと3分割したメッセージを用意して, ランダムに送信する
func TestIOMulti() {
	testHandler("3client", func() (bool, error) {
		ExpectBody, err := fileToBytes("../html/index.html")
		if err != nil {
			return false, fmt.Errorf("failt to get bytes from file")
		}
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5500",
			ReqPayload: []string{
				"GET /",
				" HTTP/1.1\r\nHost: localhost:5500\r\nUse",
				"r-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       ExpectBody,
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
			ExpectBody:       response.Content_404,
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
			ExpectBody:       response.Content_404,
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

		if err := clientB.RecvResponse(); err != nil {
			return false, err
		}
		if ok, err := clientB.IsExpectedResponse(); err != nil {
			return false, err
		} else if !ok {
			return false, nil
		}

		if err := clientC.SendPartialRequest(); err != nil {
			return false, err
		}
		if err := clientC.RecvResponse(); err != nil {
			return false, err
		}
		if ok, err := clientC.IsExpectedResponse(); err != nil {
			return false, err
		} else if !ok {
			return false, nil
		}

		if err := clientA.SendPartialRequest(); err != nil {
			return false, err
		}
		if err := clientA.RecvResponse(); err != nil {
			return false, err
		}
		if ok, err := clientA.IsExpectedResponse(); err != nil {
			return false, err
		} else if !ok {
			return false, nil
		}
		return true, nil
	})

}
