package main

import (
	"bytes"
	"fmt"
	"io"
	"log"
	"net/http"
)

// queryできるように
// 平行処理入れるか

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
		addFields      map[string]string
		wantStatusCode int
		wantBody       []byte
	}{
		{
			name:           "root",
			port:           "5500",
			url:            "/",
			addFields:      nil,
			wantStatusCode: http.StatusOK,
			wantBody:       FileContents(HELLO_WORLD_PAGE),
			//  Request:
			//    GET /no_such_file HTTP/1.1
			//    Host: localhost:5500
			//    User-Agent: Go-http-client/1.1
			//    Accept-Encoding: gzip
		},
		{
			name:           "no_such_file",
			port:           "5500",
			url:            "/no_such_file",
			addFields:      nil,
			wantStatusCode: http.StatusNotFound,
			wantBody:       FileContents(NOT_FOUND_PAGE),
			//  Request:
			//    GET /no_such_file HTTP/1.1
			//    Host: localhost:5500
			//    User-Agent: Go-http-client/1.1
			//    Accept-Encoding: gzip
		},
		{
			name: "add field",
			port: "5500",
			url:  "/",
			addFields: map[string]string{
				CONTENT_TYPE:  "txt/html",
				CONTENT_LEN:   "1", // bodyの大きさによって勝手に書き変わっているぽい
				AUTHORIZATION: "aaa",
			},
			wantStatusCode: http.StatusOK,
			wantBody:       FileContents(HELLO_WORLD_PAGE),
			//  Request:
			//    GET / HTTP/1.1
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
			resp := Request(http.MethodGet, tt.port, tt.url, tt.addFields, nil)
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
