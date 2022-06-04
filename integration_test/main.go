package main

import (
	"fmt"
	"integration_test/tests"
	"integration_test/testutils"
	"os"
	"time"
)

// TODO: 一つのクライアントから複数リクエスト->複数レスポンス, スライスとか使うか
// TODO: 限界近くの多重接続
// TODO: 失敗時の送信メッセージの表示
//"integration_test/conf/limit_expect.conf"はどこで使う？

func main() {
	var status int
	select {
	case <-time.After(30 * time.Second):
		fmt.Fprintln(os.Stderr, "itest: unexptected timeout")
		status = 1
	case <-test():
		if tests.IsFail() || tests.IsFatal() {
			status = 1
		}
	}
	testutils.KillWebserv(status != 0)
	os.Exit(status)
}

func test() chan struct{} {
	done := make(chan struct{})
	go func() {
		defer close(done)
		t := tests.Generate()
		t.Test()
	}()
	return done
}
