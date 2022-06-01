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
var log string

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
			txt := scanner.Text()
			log = log + txt + "\n"
			if txt == "start server process" {
				close(done)
				return
			}
		}
	}()
	return done
}

func KillWebserv() {
	if current_process == nil {
		return
	}
	current_process.Process.Kill()
	if tests.IsFatal() {
		str, _ := io.ReadAll(stderr)
		fmt.Println()
		fmt.Println("===webserv===")
		fmt.Printf("%s", log)
		fmt.Printf("%s\n", str)
		fmt.Println("=============")
	}
	current_process.Wait()
	current_process = nil
}
