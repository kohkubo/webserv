package webserv

import (
	"fmt"
	"integrationtest/colorprint"
	"os"
)

func ExitWithKill(err error) {
	colorprint.Stderr("Exit by unexpeted error!")
	fmt.Fprintf(os.Stderr, "error : %v\n", err)
	Kill()
	os.Exit(1)
}

func Kill() {
	if currentProcess == nil {
		return
	}
	currentProcess.Process.Kill()
	currentProcess.Wait()
	currentProcess = nil
}
