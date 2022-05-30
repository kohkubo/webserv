package main

import (
	"integration_test/tests"
	"os"
)

// TODO: POSTのテスト
// TODO: 一つのクライアントから複数リクエスト->複数レスポンス, スライスとか使うか

func main() {
	RestartWebserv("integration_test/conf/webserv.conf")

	tests.TestGET()
	//tests.TestPOST()
	tests.TestDELETE()
	tests.TestIOMULT()

	KillWebserv()
	if tests.CountTestFail != 0 {
		os.Exit(1)
	}
}
