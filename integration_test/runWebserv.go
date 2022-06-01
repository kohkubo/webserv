package main

import (
	"bufio"
	"fmt"
	"integration_test/tests"
	"io"
	"os"
	"os/exec"
	"time"
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
	select {
	case <-time.After(10 * time.Second):
		fmt.Fprintln(os.Stderr, "timout to wait server lauch")
		tests.CountTestFatal++
		return
	case <-waitServerLaunch():
	}
}

func waitServerLaunch() chan struct{} {
	done := make(chan struct{})
	scanner := bufio.NewScanner(stderr)
	go func() {
		for scanner.Scan() {
			if scanner.Text() == "start server process" {
				close(done)
				return
			}
		}
	}()
	return done
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
