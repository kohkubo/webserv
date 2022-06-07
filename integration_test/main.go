package main

import (
	"integration_test/colorprint"
	"integration_test/tests"
	"integration_test/webserv"
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
	case <-time.After(5 * time.Minute):
		colorprint.Stderr("itest: unexptected timeout")
		status = 1
	case ok := <-test():
		if ok {
			colorprint.Stdout("All ok")
		} else {
			colorprint.Stderr("Fail")
			status = 1
		}
	}
	webserv.Kill(status != 0)
	os.Exit(status)
}

func test() chan bool {
	result := make(chan bool)
	go func() {
		t := tests.Generate()
		result <- t.Test()
	}()
	return result
}
