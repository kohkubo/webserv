package main

import (
	"bytes"
	"fmt"
	"io"
	"log"
	"net/http"
)

// デフォルトで作成されるリクエスト
// リクエスト: GET http://localhost:5001
// 中身:
//  GET / HTTP/1.1
//  Host: localhost:5001
//  User-Agent: Go-http-client/1.1
//  Accept-Encoding: gzip
func testGET() {
	tests := []struct {
		name           string
		uri            string
		addFields      map[string]string
		wantStatusCode int
		wantBody       []byte
	}{
		{
			name:           "root",
			uri:            "http://localhost:5500",
			addFields:      nil,
			wantStatusCode: http.StatusOK,
			wantBody:       FileContents(HELLO_WORLD_PAGE),
		},
		{
			name:           "no_such_file",
			uri:            "http://localhost:5500/not_such_file",
			addFields:      nil,
			wantStatusCode: http.StatusNotFound,
			wantBody:       FileContents(NOT_FOUND_PAGE),
		},
	}
	fmt.Println("GET test")
	for _, tt := range tests {
		tt := tt
		func() {
			fmt.Print("[ " + tt.name + " ] ")
			var errorOccurred bool
			resp := NewRequest("GET", tt.uri, tt.addFields, nil)
			defer resp.Body.Close()
			if resp.StatusCode != tt.wantStatusCode {
				errorOccurred = true
				fmt.Printf("actual_status: %v, expect_status: %v\n", resp.StatusCode, tt.wantStatusCode)
			}
			resposenseBody, err := io.ReadAll(resp.Body)
			if err != nil {
				log.Fatalf("fail to get read body: %v", err)
			}
			if bytes.Compare(resposenseBody, tt.wantBody) != 0 {
				errorOccurred = true
				fmt.Printf("actual_body: %v, expect_body: %v\n", resp.StatusCode, tt.wantStatusCode)
			}
			if !errorOccurred {
				fmt.Println(GREEN, "ok", RESET)
			}
		}()
	}
}
