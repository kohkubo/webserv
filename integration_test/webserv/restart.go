package webserv

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"os"
	"os/exec"
	"time"
)

var currentProcess *exec.Cmd = nil

// 指定したpathのconfigファイルでwebservを立ち上げる。
func Restart(configPath string) error {
	if currentProcess != nil {
		Kill()
	}
	currentProcess = exec.Command("./webserv", configPath)
	currentProcess.Dir = "../"
	pipeRead, pipeWrite, err := os.Pipe()
	if err != nil {
		log.Fatal(err)
	}
	currentProcess.Stdout = os.Stdout
	currentProcess.Stderr = io.MultiWriter(os.Stderr, pipeWrite)
	currentProcess.Start()
	select {
	case <-time.After(10 * time.Second):
		return fmt.Errorf("timout to wait server lauch")
	case <-waitServerLaunch(pipeRead):
	}
	return nil
}

func waitServerLaunch(r io.Reader) chan struct{} {
	done := make(chan struct{})
	scanErr := bufio.NewScanner(r)
	go func() {
		for scanErr.Scan() {
			txt := scanErr.Text()
			if txt == "start server process" {
				close(done)
				return
			}
		}
	}()
	return done
}
