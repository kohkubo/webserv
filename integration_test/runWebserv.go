package main

import (
	"bufio"
	"fmt"
	"integration_test/exe"
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
		exe.CountTestFatal++
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
			log = fmt.Sprintf("%s%s\n", log, txt)
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
	if exe.IsFatal() {
		str, _ := io.ReadAll(stderr)
		log = fmt.Sprintf("%s%s\n", log, str)
		fmt.Println()
		fmt.Println("===webserv===")
		fmt.Printf("%s", log)
		fmt.Println("=============")
	}
	current_process.Wait()
	current_process = nil
}
