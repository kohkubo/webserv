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
		exe.BigHandler("DELETE", tests.TestDELETE, "integration_test/conf/webserv.conf")
		exe.BigHandler("IO Multiplexing", tests.TestIOMulti, "integration_test/conf/webserv.conf")
		exe.BigHandler("bad request", tests.TestBadRequest, "integration_test/conf/webserv.conf")

		exe.BigHandler("autoindex", tests.TestAutoindex, "integration_test/conf/autoindex.conf")

		exe.BigHandler("server_name", tests.TestServerName, "integration_test/conf/server_name.conf")

		exe.BigHandler("cgi", tests.TestCgi, "integration_test/conf/test.conf")
		exe.BigHandler("location", tests.TestLocation, "integration_test/conf/test.conf")
		exe.BigHandler("limitexept", tests.TestLimitExpect, "integration_test/conf/test.conf")

		t := tests.Generate()
		t.Test()

		//"integration_test/conf/limit_expect.conf"
	}()
	return done
}
