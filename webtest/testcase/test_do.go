package testcase

import (
	"bytes"
	"fmt"
	"io"
	"log"
	"net/http"
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

func (tc *TestCase) Do() {
	fmt.Print("[ " + tc.Name + " ] ")
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
	if resp.StatusCode != tc.WantStatusCode {
		wasErr = true
		fmt.Printf("actual_status: %v, expect_status: %v\n", resp.StatusCode, tc.WantStatusCode)
	}

	// ボディの確認
	resposenseBody, err := io.ReadAll(resp.Body)
	if err != nil {
		log.Fatalf("fail to get read body: %v", err)
	}
	if bytes.Compare(resposenseBody, tc.WantBody) != 0 {
		wasErr = true
		fmt.Printf("actual_body: %v, expect_body: %v\n", resp.StatusCode, tc.WantStatusCode)
	}

	if !wasErr {
		fmt.Println(GREEN, "ok", RESET)
	}
}
