package main

import (
	"bufio"
	"fmt"
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
		fmt.Fprintln(os.Stderr, "fail to start webserv")
		os.Exit(1)
	}
	current_process = exec.Command("./webserv", configPath)
	// itestの実行ファイルがintegration_test/integration_testを期待
	current_process.Dir = "../"
	stderr, _ = current_process.StderrPipe()
	current_process.Start()
	select {
	case <-time.After(10 * time.Second):
		fmt.Fprintln(os.Stderr, "timout to check webserv starts")
		KillWebserv(true)
		os.Exit(1)
	case <-waitServerLaunch():
	}
}

func waitServerLaunch() chan struct{} {
	done := make(chan struct{})
	go func() {
		scanner := bufio.NewScanner(stderr)
		for scanner.Scan() {
			if scanner.Text() == "start server process" {
				close(done)
				return
			}
		}
	}()
	return done
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
