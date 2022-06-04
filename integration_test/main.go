package main

import (
	"fmt"
	"integration_test/exe"
	"integration_test/tests"
	"os"
	"time"
)

// TODO: 一つのクライアントから複数リクエスト->複数レスポンス, スライスとか使うか
// TODO: 限界近くの多重接続
// TODO: 失敗時の送信メッセージの表示

func main() {
	var status int
	select {
	case <-time.After(30 * time.Second):
		fmt.Fprintln(os.Stderr, "itest: unexptected timeout")
		status = 1
	case <-test():
		if exe.IsFail() || exe.IsFatal() {
			status = 1
		}
	}
	exe.KillWebserv()
	os.Exit(status)
}

func test() chan struct{} {
	done := make(chan struct{})
	go func() {
		defer close(done)
		exe.BigHandler("POST", tests.TestPOST, "integration_test/conf/webserv.conf")

		exe.BigHandler("server_name", tests.TestServerName, "integration_test/conf/server_name.conf")

		exe.BigHandler("location", tests.TestLocation, "integration_test/conf/test.conf")

		// mainではTestメソッドを呼ぶのみ
		t := tests.Generate()
		t.Test()

		//"integration_test/conf/limit_expect.conf"
	}()
	return done
}
