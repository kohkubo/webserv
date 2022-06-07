package webserv

import (
	"bufio"
	"fmt"
	"io"
	"os/exec"
	"time"
)

var current_process *exec.Cmd = nil
var stderr io.ReadCloser
var log string

// 指定したpathのconfigファイルでwebservを立ち上げる。
func Restart(configPath string) error {
	if current_process != nil {
		Kill(false)
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
