package webserv

import (
	"bufio"
	"fmt"
	"io"
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
	pr, pw, err := os.Pipe()
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
	w := io.MultiWriter(os.Stderr, pw)
	currentProcess.Stdout = os.Stdout
	currentProcess.Stderr = w
	currentProcess.Start()
	select {
	case <-time.After(10 * time.Second):
		return fmt.Errorf("timout to wait server lauch")
	case <-waitServerLaunch(pr):
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
