package main

import (
	"bytes"
	"fmt"
	"io"
	"log"
	"net/http"
)

// デフォルトで作成されるリクエスト
// リクエスト:
//  http.NewRequest("GET", "http://localhost:5001", nil)
// 中身:
//  GET / HTTP/1.1
//  Host: localhost:5001
//  User-Agent: Go-http-client/1.1
//  Accept-Encoding: gzip
func TestGet() {
	tests := []struct {
		name           string
		uri            string
		addFields      map[string]string
		wantStatusCode int
		wantBody       []byte
	}{
		{
			name:           "root",
			uri:            "http://localhost:5001",
			addFields:      nil,
			wantStatusCode: http.StatusOK,
			wantBody:       FileContents(index_html),
		},
	}
	fmt.Println("GET test")
	for _, tt := range tests {
		tt := tt
		func() {
			resp := NewRequest("GET", tt.uri, tt.addFields, nil)
			defer resp.Body.Close()
			if resp.StatusCode != tt.wantStatusCode {
				fmt.Printf("["+tt.name+" status]"+"actual: %v, expect: %v", resp.StatusCode, tt.wantStatusCode)
			}
			resposenseBody, err := io.ReadAll(resp.Body)
			if err != nil {
				log.Fatalf("fail to get read body: %v", err)
			}
			if bytes.Compare(resposenseBody, tt.wantBody) != 0 {
				fmt.Printf("["+tt.name+" body]"+"actual: %v, expect: %v", resp.StatusCode, tt.wantStatusCode)
			}
		}()
	}
}
