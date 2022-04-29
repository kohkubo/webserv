package main

import (
	"fmt"
	"net/http"
)

const (
	protocol = "http"
	host     = "localhost"
)

// addFieldsについて
//   "Content-Length"など(おそらく)内部でbodyの長さによって書き変わっていて, 反映されないものもある
//   Request structのContent-Length変数が設定されていることは確認した

func testGET() {
	// tests: テストケース
	tests := []struct {
		name           string
		port           string
		url            string
		addQuery       map[string]string // 追加パラメータ: 'URL?foo=bar'
		addFields      map[string]string // 追加フィールド: 'Content-Type: txt/html'
		wantStatusCode int
		wantBody       []byte
	}{
		{
			name:           "root 5500",
			port:           "5500",
			url:            "/",
			wantStatusCode: http.StatusOK,
			wantBody:       FileContents(HELLO_WORLD_PAGE),
			//  Request(3,4行目はデフォルトで設定されたもの):
			//    GET / HTTP/1.1
			//    Host: localhost:5500
			//    User-Agent: Go-http-client/1.1
			//    Accept-Encoding: gzip
		},
		{
			name:           "root 5001",
			port:           "5001",
			url:            "/",
			wantStatusCode: http.StatusOK,
			wantBody:       FileContents(HELLO_WORLD_PAGE),
		},
		{
			name:           "no_such_file",
			port:           "5500",
			url:            "/no_such_file",
			wantStatusCode: http.StatusNotFound,
			wantBody:       FileContents(NOT_FOUND_PAGE),
		},
		{
			name: "add info",
			port: "5500",
			url:  "/",
			addQuery: map[string]string{
				"foo": "bar",
				"ben": "johnson",
			},
			addFields: map[string]string{
				CONTENT_TYPE:  "txt/html",
				CONTENT_LEN:   "1", // 反映されなかった(?)
				AUTHORIZATION: "aaa",
			},
			wantStatusCode: http.StatusNotFound,
			wantBody:       FileContents(NOT_FOUND_PAGE),
			//  Request:
			//    GET /?ben=johnson&foo=bar HTTP/1.1
			//    Host: localhost:5500
			//    User-Agent: Go-http-client/1.1
			//    Authorization: aaa
			//    Content-Type: txt/html
			//    Accept-Encoding: gzip
		},
	}
	// テスト実行
	fmt.Println("GET test")
	for _, tt := range tests {
		tt := tt
		tc := testCase{
			name:           tt.name,
			method:         http.MethodGet,
			port:           tt.port,
			url:            tt.url,
			body:           nil,
			addQuery:       tt.addQuery,
			addFields:      tt.addFields,
			wantStatusCode: tt.wantStatusCode,
			wantBody:       tt.wantBody,
		}
		Do(tc)
	}
}
