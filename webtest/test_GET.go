package main

import (
	"bytes"
	"fmt"
	"io"
	"log"
	"net/http"
)

const (
	protocol = "http"
	host     = "localhost"
)

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
			//  Request:
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
		func() {
			fmt.Print("[ " + tt.name + " ] ")
			var wasErr bool

			// リクエスト, レスポンスの受け取り
			resp := Request(http.MethodGet, tt.port, tt.url, tt.addQuery, tt.addFields, nil)
			defer resp.Body.Close()

			// ステータスコードの確認
			if resp.StatusCode != tt.wantStatusCode {
				wasErr = true
				fmt.Printf("actual_status: %v, expect_status: %v\n", resp.StatusCode, tt.wantStatusCode)
			}

			// ボディの確認
			resposenseBody, err := io.ReadAll(resp.Body)
			if err != nil {
				log.Fatalf("fail to get read body: %v", err)
			}
			if bytes.Compare(resposenseBody, tt.wantBody) != 0 {
				wasErr = true
				fmt.Printf("actual_body: %v, expect_body: %v\n", resp.StatusCode, tt.wantStatusCode)
			}

			if !wasErr {
				fmt.Println(GREEN, "ok", RESET)
			}
		}()
	}
}
