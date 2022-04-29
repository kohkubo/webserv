package main

import (
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
)

const (
	HELLO_WORLD_PAGE           = "../html/index.html"
	FORBIDDEN_PAGE             = "../html/forbidden.html"
	NOT_FOUND_PAGE             = "../html/not_found.html"
	BAD_REQUEST_PAGE           = "../html/bad_request.html"
	NOT_IMPLEMENTED_PAGE       = "../html/not_implemented.html"
	INTERNAL_SERVER_ERROR_PAGE = "../html/internal_server_error.html"
	UNKNOWN_ERROR_PAGE         = "../html/unknown_error.html"
)

const (
	RED   = "\033[31m"
	GREEN = "\033[32m"
	RESET = "\033[0m"
)

const (
	HOST          = "Host"
	USERAGENT     = "User-Agent"
	ACCEPT        = "Accept"
	CONTENT_LEN   = "Content-Length"
	CONTENT_TYPE  = "Content-Type"
	CONNECTION    = "Connection"
	AUTHORIZATION = "Authorization"
)

// http.Clientの説明にグローバルで使用すべきと書いてあった(詳しくは分からん)
// 毎度作り直すことによる弊害の推測:
//   作ることのオーバヘッド
//   接続のキャッシュ情報が捨てられることによるリーク
var client = &http.Client{}

const (
	PreURI = "http://localhost:"
)

type testCase struct {
	name           string
	method         string
	port           string
	url            string
	body           io.Reader
	addQuery       map[string]string // 追加パラメータ:e.g. ["foo"]="bar" ('URL?foo=bar')
	addFields      map[string]string // 追加フィールド:e.g. ["Content-Type"]="txt/html"
	wantStatusCode int
	wantBody       []byte
}

func (t testCase) NewRequest() (*http.Request, error) {
	req, err := http.NewRequest(t.method, PreURI+t.port+t.url, t.body)
	if err != nil {
		return nil, fmt.Errorf("fail to send request: %v", err)
	}

	// URLパラメータ追加
	q := req.URL.Query()
	for key, value := range t.addQuery {
		q.Add(key, value)
	}
	req.URL.RawQuery = q.Encode()

	// ヘッダーフィールド追加
	for key, value := range t.addFields {
		req.Header.Add(key, value)
	}
	return req, nil
}

//// Request: 引数で渡された情報を元にリクエストを作成します.
//func Request(method, port, url string, addQuery, addFields map[string]string, body io.Reader) *http.Response {
//	req, err := http.NewRequest(method, PreURI+port+url, body)
//	if err != nil {
//		log.Fatalf("fail to send request: %v", err)
//	}

//	// URLパラメータ追加
//	q := req.URL.Query()
//	for key, value := range addQuery {
//		q.Add(key, value)
//	}
//	req.URL.RawQuery = q.Encode()

//	// ヘッダーフィールド追加
//	for key, value := range addFields {
//		req.Header.Add(key, value)
//	}

//	resp, err := client.Do(req)
//	if err != nil {
//		log.Fatalf("fail to get response: %v", err)
//	}
//	return resp
//}

// FileContents: fileNameで指定されたパスのファイルの中身を[]byteに詰めて返します.
func FileContents(fileName string) []byte {
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
