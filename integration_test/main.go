package main

import (
	"fmt"
	"integration_test/tests"
	"integration_test/tests/utils"
	"os"
	"time"
)

// TODO: 一つのクライアントから複数リクエスト->複数レスポンス, スライスとか使うか
// TODO: 限界近くの多重接続
// TODO: 失敗時の送信メッセージの表示
//"integration_test/conf/limit_expect.conf"はどこで使う？
// TODO: killwebservの使い方を見直す
// TODO: IsFail, IsFatalの使い方見直す

func main() {
	var status int
	select {
	case <-time.After(30 * time.Second):
		fmt.Fprintln(os.Stderr, "itest: unexptected timeout")
		status = 1
	case status = <-test():
	}
	utils.KillWebserv(status != 0)
	os.Exit(status)
}

func test() chan int {
	result := make(chan int)
	go func() {
		t := tests.Generate()
		if ok := t.Test(); ok {
			result <- 0
		} else {
			result <- 1
		}
	}()
	return result
}
