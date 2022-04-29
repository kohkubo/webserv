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

// http.Clientの説明にグローバルで使用すべきと書いてあった(詳しくは分からん)
// 毎度作り直すことによる弊害の推測:
//   作ることのオーバヘッド
//   接続のキャッシュ情報が捨てられることによるリーク
var client = &http.Client{}

const (
	RED   = "\033[31m"
	GREEN = "\033[32m"
	RESET = "\033[0m"
)

func Do(tc testCase) {
	fmt.Print("[ " + tc.name + " ] ")
	var wasErr bool

	req, err := tc.NewRequest()
	if err != nil {
		log.Fatalf("failt to create request: %v", err)
	}

	// リクエスト and レスポンス受け取り
	resp, err := client.Do(req)
	if err != nil {
		log.Fatalf("fail to send request: %v", err)
	}
	defer resp.Body.Close()

	// ステータスコードの確認
	if resp.StatusCode != tc.wantStatusCode {
		wasErr = true
		fmt.Printf("actual_status: %v, expect_status: %v\n", resp.StatusCode, tc.wantStatusCode)
	}

	// ボディの確認
	resposenseBody, err := io.ReadAll(resp.Body)
	if err != nil {
		log.Fatalf("fail to get read body: %v", err)
	}
	if bytes.Compare(resposenseBody, tc.wantBody) != 0 {
		wasErr = true
		fmt.Printf("actual_body: %v, expect_body: %v\n", resp.StatusCode, tc.wantStatusCode)
	}

	if !wasErr {
		fmt.Println(GREEN, "ok", RESET)
	}
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
