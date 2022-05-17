package main

import "os"

// TODO: ステータスコードの確認
// TODO: POSTのテスト
// TODO: DELETEのテストを通るようにする
// TODO: webservがレスポンスボディを文字列で持つようになったら同様にする
// TODO: レスポンスのタイムアウト実装

var CountTestFail uint

func main() {
	testGET()
	//testPOST()
	//testDELETE()
	testIOMULT()

	if CountTestFail != 0 {
		os.Exit(1)
	}
}
