package main

import (
	"integration_test/tests"
	"os"
	"os/exec"
	"time"
)

// TODO: POSTのテスト
// TODO: 一つのクライアントから複数リクエスト->複数レスポンス, スライスとか使うか

var current_process *exec.Cmd = nil

// 指定したpathのconfigファイルでwebservを立ち上げる。
func RestartWebserv(configPath string) {
	if current_process != nil {
		KillWebserv()
	}
	current_process = exec.Command("./webserv", configPath)
	// itestの実行ファイルがintegration_test/integration_testを期待
	current_process.Dir = "../"
	current_process.Start()
	time.Sleep(time.Millisecond * 10)
}

func KillWebserv() {
	current_process.Process.Kill()
	current_process.Wait()
	current_process = nil
}

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
