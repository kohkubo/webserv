package main

import (
	"fmt"
	"integration_test/tests"
	"os"
	"time"
)

// TODO: 一つのクライアントから複数リクエスト->複数レスポンス, スライスとか使うか
// TODO: 限界近くの多重接続
// TODO: 関数名にTestいらないかも

func main() {
	select {
	case <-time.After(30 * time.Second):
		fmt.Fprintln(os.Stderr, "itest: unexptected timeout")
		os.Exit(1)
	case <-test():
		if tests.IsFail() || tests.IsFatal() {
			os.Exit(1)
		}
	}
}

func test() chan struct{} {
	done := make(chan struct{})
	go func() {
		defer close(done) // テスト終了時にチャネルを閉じることでmain()に終了を知らせる
		RestartWebserv("integration_test/conf/webserv.conf")
		tests.TestGET()
		//tests.TestPOST()
		tests.TestDELETE()
		tests.TestIOMULT()
		tests.TestBADREQ()

		RestartWebserv("integration_test/conf/autoindex.conf")
		tests.TestAUTOINDEX()

		RestartWebserv("integration_test/conf/server_name.conf")
		tests.TestServer_name()
		KillWebserv()

		RestartWebserv("integration_test/conf/test.conf")
		tests.Migration()
		KillWebserv()
	}()
	return done
}
