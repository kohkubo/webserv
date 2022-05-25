package main

import (
	"integration_test/tests"
	"os"
)

// TODO: POSTのテスト
// TODO: DELETEのテストを通るようにする
// TODO: webservがレスポンスボディを文字列で持つようになったら同様にする
// TODO: レスポンスのタイムアウト実装
// TODO: 一つのクライアントから複数リクエスト->複数レスポンス, スライスとか使うか

func main() {
	tests.TestGET()
	//tests.TestPOST()
	tests.TestDELETE()
	tests.TestIOMULT()

	if tests.CountTestFail != 0 {
		os.Exit(1)
	}
}
