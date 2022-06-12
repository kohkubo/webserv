package webserv

import (
	"bufio"
	"fmt"
	"io"
	"os/exec"
	"time"
)

var currentProcess *exec.Cmd = nil
var stdErr io.ReadCloser
var log string

// 指定したpathのconfigファイルでwebservを立ち上げる。
func Restart(configPath string) error {
	if currentProcess != nil {
		Kill()
	}
	currentProcess = exec.Command("./webserv", configPath)
	// itestの実行ファイルがintegration_test/integration_testを期待
	currentProcess.Dir = "../"
	stdErr, _ = currentProcess.StderrPipe()
	currentProcess.Start()
	select {
	case <-time.After(10 * time.Second):
		return fmt.Errorf("timout to wait server lauch")
	case <-waitServerLaunch():
	}
	return nil
}

func waitServerLaunch() chan struct{} {
	done := make(chan struct{})
	scanErr := bufio.NewScanner(stdErr)
	go func() {
		log = ""
		for scanErr.Scan() {
			txt := scanErr.Text()
			log = fmt.Sprintf("%s%s\n", log, txt)
			if txt == "start server process" {
				close(done)
				return
			}
		}
	}()
	return done
}
