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
		exe.RestartWebserv("integration_test/conf/webserv.conf")
		//tests.TestPOST()
		tests.TestDELETE()
		tests.TestIOMulti()
		tests.TestBadRequest()

		exe.RestartWebserv("integration_test/conf/autoindex.conf")
		tests.TestAutoindex()

		exe.RestartWebserv("integration_test/conf/server_name.conf")
		tests.TestServerName()

		exe.RestartWebserv("integration_test/conf/test.conf")
		tests.TestGET()
		tests.TestCgi()
		tests.TestLocation()
		tests.TestLimitExpect()

		//RestartWebserv("integration_test/conf/limit_expect.conf")
	}()
	return done
}
