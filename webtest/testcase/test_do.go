package testcase

import (
	"bytes"
	"fmt"
	"io"
	"log"
	"net/http"
)

const (
	RED   = "\033[31m"
	GREEN = "\033[32m"
	RESET = "\033[0m"
)

// note
// Clientは追加項目がなければDefaltClientを使う.
// 理由は内部でTCPコネクションを管理するRoundTripperを使用しているため.
// RoundTripperは通信の際リソースをキャッシュしていて, 複数から利用されると上手く解放できずリークする.
// Client{}を新たに宣言してもDefaultTransportを使用するので問題はないが, 必要がなければやめる.
func (tc *TestCase) Do() {
	fmt.Print("[ " + tc.Name + " ] ")
	var wasErr bool

	req, err := tc.NewRequest()
	if err != nil {
		log.Fatalf("failt to create request: %v", err)
	}

	// リクエスト and レスポンス受け取り
	resp, err := http.DefaultClient.Do(req)
	if err != nil {
		log.Fatalf("fail to send request or get response: %v", err)
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
	if tc.WantBody != nil {
		if bytes.Compare(resposenseBody, tc.WantBody) != 0 {
			wasErr = true
			fmt.Printf("actual_body: %v, expect_body: %v\n", resp.StatusCode, tc.WantStatusCode)
		}
	}

	if !wasErr {
		fmt.Println(GREEN, "ok", RESET)
	}
}
