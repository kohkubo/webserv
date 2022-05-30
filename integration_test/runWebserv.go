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
	for scanner.Scan() {
		if scanner.Text() == "start server process" {
			return
		}
	}
	// TODO: error処理
}

func KillWebserv(printErrorLog bool) {
	current_process.Process.Kill()
	if printErrorLog {
		str, _ := io.ReadAll(stderr)
		fmt.Printf("%s\n", str)
	}
	current_process.Wait()
	current_process = nil
}
