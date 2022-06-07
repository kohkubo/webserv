package webserv

import (
	"fmt"
	"integration_test/colorprint"
	"io"
	"os"
)

func ExitWithKill(err error) {
	colorprint.Stderr("Exit by unexpeted error!")
	fmt.Fprintf(os.Stderr, "error content: %v", err)
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
