package main

import (
	"integration_test/tests"
	"os"
)

// TODO: POSTのテスト
// TODO: 一つのクライアントから複数リクエスト->複数レスポンス, スライスとか使うか
// TODO: 限界近くの多重接続

func main() {
	defer func() {
		if tests.IsFail() || tests.IsFatal() {
			os.Exit(1)
		}
	}()

	RestartWebserv("integration_test/conf/webserv.conf")
	tests.TestGET()
	//tests.TestPOST()
	tests.TestDELETE()
	tests.TestIOMULT()
	tests.TestBADREQ()
	KillWebserv(tests.IsFatal())

	RestartWebserv("integration_test/conf/autoindex.conf")
	tests.TestAUTOINDEX()
	KillWebserv(tests.IsFatal())
}
