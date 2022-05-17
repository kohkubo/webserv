package main

import (
	"fmt"
)

func testGET() {
	fmt.Println("GET test")
	testHandler("simple", func() (bool, error) {
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
		if !clientA.isTestOK() {
			return false, nil
		}
		return true, nil
	})

	//tests := []testcase.TestCase{
	//	{ // sample test
	//		Name:           "sample",
	//		Port:           PORT_5500,
	//		Url:            ROOT,
	//		WantStatusCode: http.StatusOK,
	//		WantBody:       FileToBytes(HELLO_WORLD_PAGE),

	//		// リクエスト内容:
	//		//   GET / HTTP/1.1
	//		//   Host: localhost:5500
	//		//   User-Agent: Go-http-client/1.1
	//		//   Accept-Encoding: gzip
	//	},
	//	{
	//		Name:           "5001 root",
	//		Port:           PORT_5001,
	//		Url:            ROOT,
	//		WantStatusCode: http.StatusOK,
	//		WantBody:       FileToBytes(HELLO_WORLD_PAGE),
	//	},
	//	{
	//		Name:           "no_such_file",
	//		Port:           PORT_5500,
	//		Url:            NO_SUCH_FILE,
	//		WantStatusCode: http.StatusNotFound,
	//		WantBody:       FileToBytes(NOT_FOUND_PAGE),
	//	},
	//	{ // sample test: パラメータやフィールドをつけてみた
	//		Name: "add query field",
	//		Port: PORT_5001,
	//		Url:  "/hogehoge",
	//		AddQuery: map[string]string{
	//			"foo": "bar",
	//			"ben": "johnson",
	//		},
	//		AddFields: map[string]string{
	//			CONTENT_TYPE:  "txt/html",
	//			CONTENT_LEN:   "1", // bodyと矛盾して反映されないぽい
	//			AUTHORIZATION: "aaa",
	//		},
	//		WantStatusCode: http.StatusNotFound,
	//		WantBody:       FileToBytes(NOT_FOUND_PAGE),

	//		// リクエスト内容:
	//		//   GET /hogehoge?ben=johnson&foo=bar HTTP/1.1
	//		//   Host: localhost:5001
	//		//   User-Agent: Go-http-client/1.1
	//		//   Authorization: aaa
	//		//   Content-Type: txt/html
	//		//   Accept-Encoding: gzip
	//	},
	//}
	//// テスト実行
	//fmt.Println("GET test")
	//for _, t := range tests {
	//	t := t
	//	t.Method = http.MethodGet
	//	t.Do()
	//}
}
