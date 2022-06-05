package utils

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
var log string

// 指定したpathのconfigファイルでwebservを立ち上げる。
func RestartWebserv(configPath string) error {
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
		return fmt.Errorf("timout to wait server lauch")
	case <-waitServerLaunch():
	}
	return nil
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

// for color print
const (
	red   = "\033[31m"
	green = "\033[32m"
	reset = "\033[0m"
)

func ExitWithKillServer(err error) {
	fmt.Fprintf(os.Stderr, "%sExit by unexpeted error!%s: %v", red, reset, err)
	KillWebserv(true)
	os.Exit(1)
}

func KillWebserv(printLog bool) {
	if current_process == nil {
		return
	}
	current_process.Process.Kill()
	if printLog {
		str, _ := io.ReadAll(stderr)
		log = fmt.Sprintf("%s%s\n", log, str)
		fmt.Println()
		fmt.Println("===webserv===")
		fmt.Printf("%s", log)
		fmt.Println("=============")
	}
	current_process.Wait()
	current_process = nil
	log = ""
}
