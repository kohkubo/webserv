package main

import (
	"integration_test/tests"
	"os"
)

// TODO: POSTのテスト
// TODO: 一つのクライアントから複数リクエスト->複数レスポンス, スライスとか使うか
// TODO: 限界近くの多重接続

func main() {
	RestartWebserv("integration_test/conf/webserv.conf")
	defer KillWebserv(tests.IsFatal())
	defer func() {
		if tests.IsFail() {
			os.Exit(1)
		}
	}()

	// このテストの分け方は仮です。
	tests.TestGET()
	//tests.TestPOST()
	tests.TestDELETE()
	tests.TestIOMULT()
	tests.TestBADREQ()
}
