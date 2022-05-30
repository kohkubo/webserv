package main

import (
	"fmt"
	"io"
	"os/exec"
	"time"
)

var current_process *exec.Cmd = nil
var stderr io.ReadCloser

// 指定したpathのconfigファイルでwebservを立ち上げる。
func RestartWebserv(configPath string) {
	if current_process != nil {
		KillWebserv()
	}
	current_process = exec.Command("./webserv", configPath)
	// itestの実行ファイルがintegration_test/integration_testを期待
	current_process.Dir = "../"
	stderr, _ = current_process.StderrPipe()
	current_process.Start()
	time.Sleep(time.Millisecond * 10)
}

func KillWebserv() {
	current_process.Process.Kill()
	current_process.Wait()
	// if err
	str, _ := io.ReadAll(stderr)
	fmt.Printf("%s\n", str)
	current_process = nil
}
