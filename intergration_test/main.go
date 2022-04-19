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

// ヘッダーは中身なし
//http.NewRequest("GET", "http://localhost:5001", nil)
//中身:
//GET / HTTP/1.1
//Host: localhost:5001
//User-Agent: Go-http-client/1.1
//Accept-Encoding: gzip

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
// デフォルトで追加されるフィールドに関してまとめておく
// 追加のフィールドはrangeで追加する
// テーブルで実装する
func main() {
	req, err := http.NewRequest("DELETE", "http://localhost:5001", nil)
	if err != nil {
		log.Fatalf("fail to send request: %v", err)
	}
	//req.Header.Add("Range", "a")
	//for k, v := range req.Header {
	//	fmt.Println("--->", k, v)
	//}
	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		log.Fatalf("fail to get response: %v", err)
	}
	defer resp.Body.Close()
	body, err := io.ReadAll(resp.Body)
	if err != nil {
		log.Fatalf("fail to get read body: %v", err)
	}
	expect, err := FileBytes("../html/index.html")
	if err != nil {
		log.Fatalf("fail to get file bytes: %v", err)
	}
	if bytes.Compare(body, expect) != 0 {
		log.Fatal("error")
	} else {
		fmt.Println("ok")
	}
}

func FileBytes(fileName string) ([]byte, error) {
	file, err := os.Open(fileName)
	if err != nil {
		return nil, fmt.Errorf("FileBytes: %v", err)
	}
	defer file.Close()
	srcBytes, err := ioutil.ReadAll(file)
	if err != nil {
		return nil, fmt.Errorf("FileBytes: %v", err)
	}
	return srcBytes, nil
}
