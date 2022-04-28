package main

import (
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

// Request: 引数で渡された情報を元にリクエストを作成します.
// 現状詰め込み過ぎも気になって使うかわかりません.
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
