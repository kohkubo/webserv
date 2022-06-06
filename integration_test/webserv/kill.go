package webserv

import (
	"fmt"
	"io"
	"os"
)

// for color print
const (
	red   = "\033[31m"
	green = "\033[32m"
	reset = "\033[0m"
)

func ExitWithKill(err error) {
	fmt.Fprintf(os.Stderr, "%sExit by unexpeted error!%s: %v", red, reset, err)
	Kill(true)
	os.Exit(1)
}

func Kill(printLog bool) {
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
