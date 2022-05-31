package main

import (
	"bufio"
	"fmt"
	"io"
	"os/exec"
)

var current_process *exec.Cmd = nil
var stderr io.ReadCloser

// 指定したpathのconfigファイルでwebservを立ち上げる。
func RestartWebserv(configPath string) {
	if current_process != nil {
		KillWebserv(false)
	}
	current_process = exec.Command("./webserv", configPath)
	// itestの実行ファイルがintegration_test/integration_testを期待
	current_process.Dir = "../"
	stderr, _ = current_process.StderrPipe()
	current_process.Start()
	scanner := bufio.NewScanner(stderr)
	// webservが始まった瞬間終わるとこのEOFが原因でこのScanがfalse返ってしまう
	// つまりwebserv側の出力が取れない
	for scanner.Scan() {
		txt := scanner.Text()
		if txt == "start server process" {
			return
		}
	}
}

func KillWebserv(printLog bool) {
	if current_process == nil {
		return
	}
	current_process.Process.Kill()
	if printLog {
		str, _ := io.ReadAll(stderr)
		fmt.Printf("%s\n", str)
	}
	current_process.Wait()
	current_process = nil
}
