package main

import (
	"bytes"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
)

//http.NewRequest("POST", "http://localhost:5001", nil)
//中身:
//POST / HTTP/1.1
//Host: localhost:5001
//User-Agent: Go-http-client/1.1
//Content-Length: 0
//Accept-Encoding: gzip

//http.NewRequest("DELETE", "http://localhost:5001", nil)
//中身:
//DELETE / HTTP/1.1
//Host: localhost:5001
//User-Agent: Go-http-client/1.1
//Accept-Encoding: gzip

// get, post, deleteでテストを分ける(postではタイプが必要か？)
const (
	index_html = "../html/index.html"
)

func main() {
	TestGet()
}

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
			wantBody:       FileBytes(index_html),
		},
	}
	fmt.Println("GET test")
	for _, tt := range tests {
		tt := tt
		func() {
			resp := Request("GET", tt.uri, tt.addFields, nil)
			defer resp.Body.Close()
			if resp.StatusCode != tt.wantStatusCode {
				fmt.Printf("["+tt.name+" status]"+"actual: %v, expect: %v", resp.StatusCode, tt.wantStatusCode)
			}
			bodyResponsed, err := io.ReadAll(resp.Body)
			if err != nil {
				log.Fatalf("fail to get read body: %v", err)
			}
			if bytes.Compare(bodyResponsed, tt.wantBody) != 0 {
				fmt.Printf("["+tt.name+" body]"+"actual: %v, expect: %v", resp.StatusCode, tt.wantStatusCode)
			}
		}()
	}
}

func Request(method string, uri string, addFields map[string]string, body io.Reader) *http.Response {
	req, err := http.NewRequest("GET", uri, body)
	if err != nil {
		log.Fatalf("fail to send request: %v", err)
	}
	for key, value := range addFields {
		req.Header.Add(key, value)
	}
	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		log.Fatalf("fail to get response: %v", err)
	}
	return resp
}

func FileBytes(fileName string) []byte {
	file, err := os.Open(fileName)
	if err != nil {
		log.Fatalf("FileBytes: %v", err)
	}
	defer file.Close()
	srcBytes, err := ioutil.ReadAll(file)
	if err != nil {
		log.Fatalf("FileBytes: %v", err)
	}
	return srcBytes
}
