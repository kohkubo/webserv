package main

import (
	"fmt"
	"net/http"
	"webtest/testcase"
)

func testGET() {
	tests := []testcase.TestCase{
		{ // base test
			Name:           "5500 root",
			Port:           PORT_5500,
			Url:            ROOT,
			WantStatusCode: http.StatusOK,
			WantBody:       FileContents(HELLO_WORLD_PAGE),

			// リクエスト内容:
			//   GET / HTTP/1.1
			//   Host: localhost:5500
			//   User-Agent: Go-http-client/1.1
			//   Accept-Encoding: gzip
		},
		{
			Name:           "5001 root",
			Port:           PORT_5001,
			Url:            ROOT,
			WantStatusCode: http.StatusOK,
			WantBody:       FileContents(HELLO_WORLD_PAGE),
		},
		{
			Name:           "no_such_file",
			Port:           PORT_5500,
			Url:            NO_SUCH_FILE,
			WantStatusCode: http.StatusNotFound,
			WantBody:       FileContents(NOT_FOUND_PAGE),
		},
		{
			Name: "add query field",
			Port: PORT_5001,
			Url:  "/hogehoge",
			AddQuery: map[string]string{
				"foo": "bar",
				"ben": "johnson",
			},
			AddFields: map[string]string{
				CONTENT_TYPE:  "txt/html",
				CONTENT_LEN:   "1", // bodyと矛盾して反映されないぽい
				AUTHORIZATION: "aaa",
			},
			WantStatusCode: http.StatusOK,
			WantBody:       FileContents(NOT_FOUND_PAGE),

			// リクエスト内容:
			//   GET /hogehoge?ben=johnson&foo=bar HTTP/1.1
			//   Host: localhost:5001
			//   User-Agent: Go-http-client/1.1
			//   Authorization: aaa
			//   Content-Type: txt/html
			//   Accept-Encoding: gzip
		},
	}
	// テスト実行
	fmt.Println("GET test")
	for _, t := range tests {
		t := t
		t.Method = http.MethodGet
		t.Do()
	}
}
